///////////////////////////////////////////////////////////////////////////////
// File: DDTIBRadCableAlgo.cc
// Description: Equipping the side disks of TIB with cables etc
///////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <algorithm>

namespace std{} using namespace std;
#include "DetectorDescription/Parser/interface/DDLParser.h"
#include "DetectorDescription/Base/interface/DDdebug.h"
#include "DetectorDescription/Base/interface/DDutils.h"
#include "DetectorDescription/Core/interface/DDPosPart.h"
#include "DetectorDescription/Core/interface/DDLogicalPart.h"
#include "DetectorDescription/Core/interface/DDSolid.h"
#include "DetectorDescription/Core/interface/DDMaterial.h"
#include "DetectorDescription/Core/interface/DDCurrentNamespace.h"
#include "DetectorDescription/Core/interface/DDSplit.h"
#include "Geometry/TrackerCommonData/interface/DDTIBRadCableAlgo.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"


DDTIBRadCableAlgo::DDTIBRadCableAlgo(): layRin(0),cableMat(0),strucMat(0) {
  DCOUT('a', "DDTIBRadCableAlgo info: Creating an instance");
}

DDTIBRadCableAlgo::~DDTIBRadCableAlgo() {}

void DDTIBRadCableAlgo::initialize(const DDNumericArguments & nArgs,
				   const DDVectorArguments & vArgs,
				   const DDMapArguments & ,
				   const DDStringArguments & sArgs,
				   const DDStringVectorArguments & vsArgs) {

  idNameSpace  = DDCurrentNamespace::ns();
  unsigned int i;
  DDName parentName = parent().name();
  DCOUT('A', "DDTIBRadCableAlgo debug: Parent " << parentName << " NameSpace " << idNameSpace);

  rMin         = nArgs["RMin"];
  rMax         = nArgs["RMax"];
  layRin       = vArgs["RadiusLo"];    
  deltaR       = nArgs["DeltaR"];
  DCOUT('A', "DDTIBRadCableAlgo debug: Disk Rmin " << rMin << "\tRMax " << rMax  << "\tSeparation of layers " << deltaR	<< " with " << layRin.size() << " layers at R =");
  for (i = 0; i < layRin.size(); i++)
    DCOUT('A', "\t " << layRin[i]);

  cylinderT    = nArgs["CylinderThick"];
  supportT     = nArgs["SupportThick"];
  supportDR    = nArgs["SupportDR"];
  supportMat   = sArgs["SupportMaterial"];
  DCOUT('A', "DDTIBRadCableAlgo debug: SupportCylinder Thickness " << cylinderT << "\tSupportDisk Thickness " << supportT << "\tExtra width along R " << supportDR << "\tMaterial: " << supportMat);

  cableT       = nArgs["CableThick"];     
  cableMat     = vsArgs["CableMaterial"];
  DCOUT('A', "DDTIBRadCableAlgo debug: Cable Thickness " << cableT << " with materials: ");
  for (i = 0; i < cableMat.size(); i++)
    DCOUT('A', "\t " << cableMat[i]);

  strucMat     = vsArgs["StructureMaterial"];
  DCOUT('A', "DDTIBRadCableAlgo debug: " << strucMat.size() << " materials for open structure:");
  for (i=0; i<strucMat.size(); i++)
    DCOUT('A', "\t " << strucMat[i]);
}

void DDTIBRadCableAlgo::execute() {
  
  DCOUT('a', "==>> Constructing DDTIBRadCableAlgo...");
  DDName diskName = parent().name();

  DDSolid solid;
  string  name;
  double  rin, rout;

  // Loop over sub disks
  DDName suppName(DDSplit(supportMat).first, DDSplit(supportMat).second);
  DDMaterial suppMatter(suppName);
  double diskDz = 0.5 * (supportT + cableT*layRin.size());
  double dz     = 0.5*supportT;

  for (unsigned int i=0; i<layRin.size(); i++) {

    //Support disks
    name  = "TIBSupportSideDisk" + dbl_to_string(i);
    rin   = layRin[i]+0.5*(deltaR-cylinderT)-supportDR;
    rout  = layRin[i]+0.5*(deltaR+cylinderT)+supportDR;
    solid = DDSolidFactory::tubs(DDName(name, idNameSpace), dz, rin, 
				 rout, 0, twopi);
    DCOUT('a', "DDTIBRadCableAlgo test: " << DDName(name, idNameSpace) << " Tubs made of " << supportMat << " from 0 to " << twopi/deg << " with Rin " << rin << " Rout " << rout << " ZHalf " << dz);
    DDLogicalPart suppLogic(DDName(name, idNameSpace), suppMatter, solid);

    DDTranslation r1(0, 0, (dz-diskDz));
    DDpos(DDName(name,idNameSpace), diskName, i+1, r1, DDRotation());
    DCOUT('a', "DDTIBRadCableAlgo test: " << DDName(name,idNameSpace) << " number " << i+1 << " positioned in " << diskName << " at " << r1 << " with no rotation");
    
    //Open Structure
    name  = "TIBOpenZone" + dbl_to_string(i);
    rout  = rin;
    if (i == 0) rin = rMin;
    else        rin = layRin[i-1]+0.5*(deltaR+cylinderT)+supportDR;
    solid = DDSolidFactory::tubs(DDName(name, idNameSpace), dz, rin, 
				 rout, 0, twopi);
    DCOUT('a', "DDTIBRadCableAlgo test: " << DDName(name, idNameSpace) << " Tubs made of " << strucMat[i] << " from 0 to " << twopi/deg << " with Rin " << rin << " Rout " << rout << " ZHalf " << dz);
    DDName strucName(DDSplit(strucMat[i]).first, DDSplit(strucMat[i]).second);
    DDMaterial strucMatter(strucName);
    DDLogicalPart strucLogic(DDName(name, idNameSpace), strucMatter, solid);

    DDTranslation r2(0, 0, (dz-diskDz));
    DDpos(DDName(name,idNameSpace), diskName, i+1, r2, DDRotation());
    DCOUT('a', "DDTIBRadCableAlgo test: " << DDName(name,idNameSpace) << " number " << i+1 << " positioned in " << diskName << " at " << r2 << " with no rotation");

    //Now the radial cable
    name  = "TIBRadCable" + dbl_to_string(i);
    double rv = layRin[i]+0.5*deltaR;
    vector<double> pgonZ;
    pgonZ.push_back(-0.5*cableT); 
    pgonZ.push_back(cableT*(rv/rMax-0.5));
    pgonZ.push_back(0.5*cableT);
    vector<double> pgonRmin;
    pgonRmin.push_back(rv); 
    pgonRmin.push_back(rv); 
    pgonRmin.push_back(rv); 
    vector<double> pgonRmax;
    pgonRmax.push_back(rMax); 
    pgonRmax.push_back(rMax); 
    pgonRmax.push_back(rv); 
    solid = DDSolidFactory::polycone(DDName(name, idNameSpace), 0, twopi,
				     pgonZ, pgonRmin, pgonRmax);
    DCOUT('a', "DDTIBRadCableAlgo test: " << DDName(name, idNameSpace) << " Polycone made of " << cableMat[i] << " from 0 to " << twopi/deg << " and with " << pgonZ.size() << " sections");
    for (unsigned int ii = 0; ii <pgonZ.size(); ii++) 
      DCOUT('a', "\t" << "\tZ = " << pgonZ[ii] << "\tRmin = " << pgonRmin[ii] << "\tRmax = " << pgonRmax[ii]);
    DDName cableName(DDSplit(cableMat[i]).first, DDSplit(cableMat[i]).second);
    DDMaterial cableMatter(cableName);
    DDLogicalPart cableLogic(DDName(name, idNameSpace), cableMatter, solid);

    DDTranslation r3(0, 0, (diskDz-(i+0.5)*cableT));
    DDpos(DDName(name,idNameSpace), diskName, i+1, r3, DDRotation());
    DCOUT('a', "DDTIBRadCableAlgo test: " << DDName(name,idNameSpace) << " number " <<i+1 << " positioned in " << diskName << " at " << r3 << " with no rotation");
    
  }

  //Now the last open zone
  unsigned int i = layRin.size();
  rin  = layRin[i-1]+0.5*(deltaR+cylinderT)+supportDR;
  rout = rMax;
  name  = "TIBOpenZone" + dbl_to_string(i);
  solid = DDSolidFactory::tubs(DDName(name, idNameSpace), dz, rin, 
			       rout, 0, twopi);
  DCOUT('a', "DDTIBRadCableAlgo test: " << DDName(name, idNameSpace) << " Tubs made of " << strucMat[i] << " from 0 to " << twopi/deg << " with Rin " << rin << " Rout " << rout << " ZHalf " << dz);
  DDName strucName(DDSplit(strucMat[i]).first, DDSplit(strucMat[i]).second);
  DDMaterial strucMatter(strucName);
  DDLogicalPart strucLogic(DDName(name, idNameSpace), strucMatter, solid);

  DDTranslation r2(0, 0, (dz-diskDz));
  DDpos(DDName(name,idNameSpace), diskName, i+1, r2, DDRotation());
  DCOUT('a', "DDTIBRadCableAlgo test: " << DDName(name,idNameSpace) << " number " << i+1 << " positioned in " << diskName << " at " << r2 << " with no rotation");

  DCOUT('a', "<<== End of DDTIBRadCableAlgo construction ...");
}