///////////////////////////////////////////////////////////////////////////////
// File: DDTECOptoHybAlgo.cc
// Description: Placing cooling pieces in the petal material of a TEC petal
///////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <algorithm>

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DetectorDescription/Base/interface/DDutils.h"
#include "DetectorDescription/Core/interface/DDPosPart.h"
#include "DetectorDescription/Core/interface/DDCurrentNamespace.h"
#include "DetectorDescription/Core/interface/DDSplit.h"
#include "Geometry/TrackerCommonData/interface/DDTECOptoHybAlgo.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"


DDTECOptoHybAlgo::DDTECOptoHybAlgo(): angles(0) {
  edm::LogInfo("TECGeom") << "DDTECOptoHybAlgo info: Creating an instance";
}

DDTECOptoHybAlgo::~DDTECOptoHybAlgo() {}

void DDTECOptoHybAlgo::initialize(const DDNumericArguments & nArgs,
				  const DDVectorArguments & vArgs,
				  const DDMapArguments & ,
				  const DDStringArguments & sArgs,
				  const DDStringVectorArguments & ) {

  idNameSpace  = DDCurrentNamespace::ns();
  childName    = sArgs["ChildName"];

  DDName parentName = parent().name(); 

  LogDebug("TECGeom") << "DDTECOptoHybAlgo debug: Parent " << parentName 
		      << " Child " << childName << " NameSpace " <<idNameSpace;

  rmin           = nArgs["Rmin"];
  rmax           = nArgs["Rmax"];
  zpos           = nArgs["Zpos"];
  startCopyNo    = int (nArgs["StartCopyNo"]);
  angles         = vArgs["Angles"];

  LogDebug("TECGeom") << "DDTECOptoHybAlgo debug: Rmin " << rmin 
		      << " Rmax " << rmax << " Zpos " << zpos 
		      << " StartCopyNo " << startCopyNo << " Number " 
		      << angles.size();

  for (int i = 0; i < (int)(angles.size()); i++)
    LogDebug("TECGeom") << "\tAngles[" << i << "] = " << angles[i];

}

void DDTECOptoHybAlgo::execute() {
  
  LogDebug("TECGeom") << "==>> Constructing DDTECOptoHybAlgo...";

  DDName mother = parent().name();
  DDName child  = DDName(DDSplit(childName).first, DDSplit(childName).second);

  int    copyNo = startCopyNo;
  double rpos   = 0.5*(rmin+rmax);
  for (int i = 0; i < (int)(angles.size()); i++) {
    double phix = angles[i];
    double xpos = rpos * cos(phix);
    double ypos = rpos * sin(phix);
    DDTranslation tran(xpos, ypos, zpos);

    DDRotation rotation;
    double phiy = phix + 90.*deg;
    double phideg = phix/deg;
    if (phideg != 0) {
      std::string rotstr= DDSplit(childName).first+dbl_to_string(phideg*1000.);
      rotation = DDRotation(DDName(rotstr, idNameSpace));
      if (!rotation) {
	double theta = 90.*deg;
	LogDebug("TECGeom") << "DDTECOptoHybAlgo test: Creating a new "
			    << "rotation: " << rotstr << "\t90., " 
			    << phix/deg << ", 90.," << phiy/deg <<", 0, 0";
	rotation = DDrot(DDName(rotstr, idNameSpace), theta, phix, theta, phiy,
			 0., 0.);
      }
    }

    DDpos (child, mother, copyNo, tran, rotation);
    LogDebug("TECGeom") << "DDTECOptoHybAlgo test " << child << " number " 
			<< copyNo << " positioned in " << mother << " at "
			<< tran  << " with " << rotation;
    copyNo++;
  }
  
  LogDebug("TECGeom") << "<<== End of DDTECOptoHybAlgo construction ...";
}
