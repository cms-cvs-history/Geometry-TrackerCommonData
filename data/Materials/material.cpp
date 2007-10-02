#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/sax/HandlerBase.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/XMLChar.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/framework/LocalFileFormatTarget.hpp"

#include <string>
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

XERCES_CPP_NAMESPACE_USE

/** Loop over the document and find the node that matches the material, return its density */
double find_density(DOMNode * node, const char * material)
{
  double density = 0;
  char * elementName = 0;
  char * densityName = 0;
  bool has_children = node->getFirstChild() != 0;
  // only process element nodes, since they contain material description
  if (node->getNodeType() == DOMNode::ELEMENT_NODE) {
    char * tagname = XMLString::transcode(node->getNodeName());
    // check for elementary or composite material
    bool foundIt = false;
    if (!strcmp(tagname, "ElementaryMaterial") || !strcmp(tagname, "CompositeMaterial")) {
      // ok, inside a material definition. Let's get its name and density (which are attributes)
      DOMNamedNodeMap * attributes = node->getAttributes();
      if (attributes) {
	for (int i = 0; i < attributes->getLength(); i++) {
	  DOMNode* attribute = attributes->item(i);
	  char * name = XMLString::transcode(attribute->getNodeName());
	  if (!strcmp(name, "name")) {
	    elementName = XMLString::transcode(attribute->getNodeValue());
	  }
	  XMLString::release(&name);
	  name = XMLString::transcode(attribute->getNodeName());
	  if (!strcmp(name, "density")) {
	    densityName = XMLString::transcode(attribute->getNodeValue());
	  }
	  XMLString::release(&name);
	}
      }
      // after all attributes have been parsed, and we know name and density,
      // check if we have the correct material
      if (!strcmp(elementName, material)) {
	// OK, found. Now let us look for the unit
	char * pos = strchr(densityName, '*');
	if (pos == 0) {
	  throw (const char *) "Density has no unit";
	}
	*pos = '\0';
	pos++;
	// check unit
	if (!strcmp(pos, "g/cm3")) {
	  density = atof(densityName);
	} 
	else if (!strcmp(pos, "mg/cm3") || !strcmp(pos, "kg/m3")) {
	  density = atof(densityName)/1000.;
	} 
	else 
	  throw (const char *) "Density has no unit";
	// Stop the recursion
	return density;
      }
    }
    XMLString::release(&tagname);
    // OK, we did not find the material, so let us find the next material
    for (DOMNode * child = node->getFirstChild(); child != 0; child=child->getNextSibling())
    {
      double val = find_density(child, material);
      // check if the recursion stopped, then we need to return the density, too
      if (val)
	return val;
    }
  }
  else {
    // ignore all other nodes
//   DOMNode::TEXT_NODE:
//   DOMNode::ATTRIBUTE_NODE:
//   DOMNode::CDATA_SECTION_NODE:
//   DOMNode::ENTITY_REFERENCE_NODE:
//   DOMNode::ENTITY_NODE:
//   DOMNode::PROCESSING_INSTRUCTION_NODE:
//   DOMNode::COMMENT_NODE:
//   DOMNode::DOCUMENT_NODE:
//   DOMNode::DOCUMENT_TYPE_NODE:
//   DOMNode::DOCUMENT_FRAGMENT_NODE:
//   DOMNode::NOTATION_NODE:
  }
  return 0;
}

DOMElement* create_element(DOMDocument* doc, string name)
{
   XMLCh* xname = new XMLCh[name.length() + 1];
   XMLString::transcode(name.c_str(), xname, name.length());
   DOMElement* r = doc->createElement(xname);
   delete xname;
   return r;
}

void set_attribute(DOMElement * element, string name, string value)
{
   XMLCh* xname = new XMLCh[name.length() + 1];
   XMLString::transcode(name.c_str(), xname, name.length());
   XMLCh* xvalue = new XMLCh[value.length() + 1];
   XMLString::transcode(value.c_str(), xvalue, value.length());
   element->setAttribute(xname, xvalue);
   delete xname;
   delete xvalue;
}

// boilerplate DOM loading example.
int main(int argc, char *argv[])
{
   const string outFileName = "mixtedMaterials.xml";

  if (argc != 4) {
    cerr << "material material_file.xml mixture_file.txt density_file.txt" << endl;
    return -1;
  }

  // initialize parser
  try {
    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cout << "Error during initialization! :\n"
	 << message << "\n";
    XMLString::release(&message);
    return 1;
  }

  // parse document
  XercesDOMParser* parser = new XercesDOMParser();
  parser->setValidationScheme(XercesDOMParser::Val_Always);    // optional.
  parser->setDoNamespaces(true);    // optional

  ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
  parser->setErrorHandler(errHandler);

  try {
    parser->parse(argv[1]);
  }
  catch (const XMLException& toCatch) {
    char* message = XMLString::transcode(toCatch.getMessage());
    cerr << "Exception message is: \n"
	 << message << "\n";
    XMLString::release(&message);
    return -1;
  }
  catch (const DOMException& toCatch) {
    char* message = XMLString::transcode(toCatch.msg);
    cerr << "Exception message is: \n"
	 << message << "\n";
    XMLString::release(&message);
    return -1;
  }
  catch (...) {
    cerr << "Unexpected Exception during parsing\n" ;
    return -1;
  }
  DOMDocument * pdoc = parser->getDocument();
  DOMElement * root = pdoc->getDocumentElement();

  //init variables
  char buffer[256];
  int warnings =0; //number of warnings
  int toRead =0;// number of constituents that are not read
  float density_combined; //desety of the material
  float aktFraction;
  double sumFraction =100;
  string name_combined;
  // double volume_combined = 0;
  vector<string> name_constituent;
  vector<double> fraction_constituent;
  vector<string> densityName; // names of the material with altered density
  vector<double> density; //desity of the material with altered density

  //  vector<double> density_constituent;

  //open density file 
  ifstream densfile(argv[3]);
  if (!densfile.is_open()) {
    cerr << "Cound not open file " << argv[3] << endl;
    return -2;
  }  
  while (densfile.getline(buffer, 256) && densfile.good()) {
    char * pos = strchr(buffer, '#');
	if (pos) {
      *pos = '\0';
    }
    if (strlen(buffer) == 0) 
      continue;
	float dens;
	char name[256];
	int result = sscanf(buffer,"%s %f", & name, &dens);
	//cout << name <<" with " <<dens <<endl;
	if (result != 2) {
	  cerr << "wrong format encountered in "<<argv[3] << endl;
	  break;
	}
	densityName.push_back(name);
	density.push_back(dens);
  }
  

  // open input file
  ifstream infile(argv[2]);
  if (!infile.is_open()) {
    cerr << "Cound not open file " << argv[2] << endl;
    return -2;
  }


  while (infile.getline(buffer, 256) && infile.good()) {
//     cout << "read line: " << buffer << endl;
    // remove comments from string
    char *pos = strchr(buffer, '#');
    if (pos) {
      *pos = '\0';
    }
    char name[256];
    float value;
	float oldDens = 0;
 
  
    if (strlen(buffer) == 0) 
      continue;
    if(toRead == 0){
	  //  char bufo[200];
	  int result = sscanf(buffer, " \" %[^ \"] \" -%i %f", & name, &toRead ,&density_combined);
	  for( int i=0;i < densityName.size();i++){
		//cout << density[i] << endl;
		
		if(name==densityName[i]){
		  oldDens = density_combined;
		  density_combined = density[i];
		}
	  }
	  cout << "+ " << name << "\t\t(with " << toRead << " constituent(s) and density " <<density_combined<<"g/mm^3";
	  if ( oldDens != 0) cout <<" corrected from "<< oldDens <<" by "<< fabs(oldDens - density_combined)/density_combined *100<<"%";
	  cout << ")"<<endl;
	  
	  if (result != 3) {
		cerr << "wrong format encountered" << endl;
		break;
	  }
	  
	  sumFraction =0;
	  name_combined = name;
	  
	}else if(toRead > 0){
	  toRead--;
	  sscanf(buffer, " \" %[^\"] \" -%f", & name, &aktFraction);
	  sumFraction+= aktFraction;
	  cout <<"|---- "<<aktFraction<<"% "<<name<<endl;
	  name_constituent.push_back(name);
	  fraction_constituent.push_back(aktFraction/100);

	  //all constituent read?
	  if(toRead == 0) {
		//some sanity tests
		if(fabs(sumFraction -100) > 0.01){
		  cerr << "WARNING! the sum of the fractions adds not up to 100% !(is "<< sumFraction <<"%)"<<endl;
		  warnings++;
		}
		//save the material
		try {
		  // create new elements
		  // Start with mother node
		  DOMElement * composite = create_element(pdoc, "CompositeMaterial");
		  set_attribute(composite, "method", "mixture by weight");
		  set_attribute(composite, "symbol", " ");
		  char buffer[256];
		  sprintf(buffer, "%f*g/cm3", density_combined);
		  set_attribute(composite, "density", buffer);
		  set_attribute(composite, "name", name_combined);
		  // now insert child nodes
		  for (int i = 0; i < name_constituent.size(); i++) {
			// material fraction
			DOMElement * fraction = create_element(pdoc, "MaterialFraction");
			sprintf(buffer, "%f", fraction_constituent[i]);
			set_attribute(fraction, "fraction", buffer);
			// reference to material
			DOMElement * ref_material = create_element(pdoc, "rMaterial");
			if(name_constituent[i] == "TEC_Nomex")	sprintf(buffer, "tecmaterial:%s", name_constituent[i].c_str());
			else if(name_constituent[i] == "TOB_DOH")	sprintf(buffer, "tobmaterial:%s", name_constituent[i].c_str());
			else if(name_constituent[i] == "T_Ribbon12xMUConn")	sprintf(buffer, "trackermaterial:%s", name_constituent[i].c_str());
			else if(name_constituent[i] == "Optical_Fiber")	sprintf(buffer, "trackermaterial:%s", name_constituent[i].c_str());
			else if(name_constituent[i] == "CAB_Al36")	sprintf(buffer, "trackermaterial:%s", name_constituent[i].c_str());
			else sprintf(buffer, "materials:%s", name_constituent[i].c_str());
			set_attribute(ref_material, "name", buffer);
			// insert reference into material
			fraction->appendChild(ref_material);
			// inserte component into composite
			composite->appendChild(fraction);
		  }
		  // now insert into document
		  DOMNode * node = pdoc->getFirstChild();
		  node = node->getChildNodes()->item(1);		  
		  //node = node->getfirst(); 
		  if (node == NULL) cout << "RABBA"<<endl;
		  
		  // DOMNode * node = pdoc->getElementsByTagName("MaterialSection")->item(0);
		  
		  node->appendChild(composite);
		}
		catch (const DOMException& toCatch) {
		  char* message = XMLString::transcode(toCatch.msg);
		  cout << "Exception message is: \n"
			   << message << "\n";
		  XMLString::release(&message);
		  return -1;
		}
		//clear the sonstituents after writing...
		name_constituent.clear();
		fraction_constituent.clear();
	  }
	}
	
	if(toRead <0){
	  cerr << "to many constituent read!"<<endl;
	}
  }
  
  try{
	// create write
	DOMImplementation* implementation = DOMImplementation::getImplementation();
	DOMWriter* writer = implementation->createDOMWriter();
	writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
	
	// local file target
	XMLCh* filename = new XMLCh[outFileName.size()+1];
	XMLString::transcode(outFileName.c_str(), filename, outFileName.size());
	XMLFormatTarget* out = new LocalFileFormatTarget(filename);
	delete filename;
	
	// save document
	writer->writeNode(out, *pdoc);
	cout << "writing to " << outFileName << endl;
  }
  catch (const DOMException& toCatch) {
	char* message = XMLString::transcode(toCatch.msg);
	cout << "Exception message is: \n"
		 << message << "\n";
	XMLString::release(&message);
	return -1;
  }
    
  infile.close();
  cout << "done parsing..." << endl;
  // done!
  if(warnings > 0) cout << "There have been "<<warnings<<" Warnings!"<<endl;  
  
  delete parser;
  delete errHandler;
  XMLPlatformUtils::Terminate();
  return 0;
}
