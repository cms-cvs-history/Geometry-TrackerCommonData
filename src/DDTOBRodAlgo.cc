///////////////////////////////////////////////////////////////////////////////
// File: DDTOBRodAlgo.cc
// Description: Positioning constituents of a TOB rod
///////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <algorithm>

namespace std{} using namespace std;
#include "DetectorDescription/Parser/interface/DDLParser.h"
#include "DetectorDescription/Base/interface/DDdebug.h"
#include "DetectorDescription/Core/interface/DDPosPart.h"
#include "DetectorDescription/Core/interface/DDLogicalPart.h"
#include "DetectorDescription/Core/interface/DDSolid.h"
#include "DetectorDescription/Core/interface/DDMaterial.h"
#include "DetectorDescription/Core/interface/DDCurrentNamespace.h"
#include "DetectorDescription/Core/interface/DDSplit.h"
#include "Geometry/TrackerCommonData/interface/DDTOBRodAlgo.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"


DDTOBRodAlgo::DDTOBRodAlgo():
  sideRod(0),sideRodX(0),sideRodY(0),sideRodZ(0),endRod1Y(0),endRod1Z(0),
  clampX(0),clampZ(0),sideCoolX(0),sideCoolZ(0),optFibreX(0),optFibreZ(0),
  sideClampX(0),sideClamp1DZ(0),sideClamp2DZ(0), moduleRot(0),moduleY(0),
  moduleZ(0),connect(0),connectY(0),connectZ(0) {
  DCOUT('a', "DDTOBRodAlgo info: Creating an instance");
}

DDTOBRodAlgo::~DDTOBRodAlgo() {}

void DDTOBRodAlgo::initialize(const DDNumericArguments & nArgs,
			      const DDVectorArguments & vArgs,
			      const DDMapArguments & ,
			      const DDStringArguments & sArgs,
			      const DDStringVectorArguments & vsArgs) {

  central      = sArgs["CentralName"];
  shift        = nArgs["Shift"];
  idNameSpace  = DDCurrentNamespace::ns();
  unsigned int i;
  DDName parentName = parent().name();
  DCOUT('A', "DDTOBRodAlgo debug: Parent " << parentName << " Central " << central << " NameSpace " << idNameSpace << "\tShift " << shift);

  sideRod      = vsArgs["SideRodName"];     
  sideRodX     = vArgs["SideRodX"];    
  sideRodY     = vArgs["SideRodY"];
  sideRodZ     = vArgs["SideRodZ"];
  for (i=0; i<sideRod.size(); i++) {
    DCOUT('A', "DDTOBRodAlgo debug: " << sideRod[i] << " to be positioned " << sideRodX.size() <<" times at y = " << sideRodY[i] << " z = " << sideRodZ[i] << " and x =");
    for (unsigned int j=0; j<sideRodX.size(); j++)
      DCOUT('A', "\t(" << i << ") " << sideRodX[j]);
  }

  endRod1      = sArgs["EndRod1Name"];     
  endRod1Y     = vArgs["EndRod1Y"];    
  endRod1Z     = vArgs["EndRod1Z"];    
  DCOUT('A', "DDTOBRodAlgo debug: " << endRod1 << " to be positioned " << endRod1Y.size() << " times at");
  for (i=0; i<endRod1Y.size(); i++)
    DCOUT('A', "\t" << i << "\ty = " << endRod1Y[i] << "\tz = " << endRod1Z[i]);

  endRod2      = sArgs["EndRod2Name"];     
  endRod2Y     = nArgs["EndRod2Y"];    
  endRod2Z     = nArgs["EndRod2Z"];    
  DCOUT('A', "DDTOBRodAlgo debug: " << endRod2 << " to be positioned at y = " << endRod2Y << " z = " << endRod2Z);

  cable        = sArgs["CableName"];       
  cableZ       = nArgs["CableZ"];      
  DCOUT('A', "DDTOBRodAlgo debug: " << cable << " to be positioned at z = " << cableZ);

  clamp        = sArgs["ClampName"];       
  clampX       = vArgs["ClampX"];      
  clampZ       = vArgs["ClampZ"];      
  DCOUT('A', "DDTOBRodAlgo debug: " << clamp << " to be positioned " << clampX.size() << " times at");
  for (i=0; i<clampX.size(); i++)
    DCOUT('A', "\t" << i << "\tx = " << clampX[i] << "\tz = " << clampZ[i]);

  sideCool     = sArgs["SideCoolName"];    
  sideCoolX    = vArgs["SideCoolX"];   
  sideCoolZ    = vArgs["SideCoolZ"];   
  DCOUT('A', "DDTOBRodAlgo debug: " << sideCool << " to be positioned " << sideCoolX.size() << " times at");
  for (i=0; i<sideCoolX.size(); i++)
    DCOUT('A', "\t" << i << "\tx = " << sideCoolX[i] << "\tz = " << sideCoolZ[i]);

  endCool      = sArgs["EndCoolName"];     
  endCoolZ     = nArgs["EndCoolZ"];    
  endCoolRot   = sArgs["EndCoolRot"];   
  DCOUT('A', "DDTOBRodAlgo debug: " <<endCool <<" to be positioned with " << endCoolRot << " rotation at" << " z = " << endCoolZ);

  optFibre     = sArgs["OptFibreName"];    
  optFibreX    = vArgs["optFibreX"];   
  optFibreZ    = vArgs["optFibreZ"];   
  DCOUT('A', "DDTOBRodAlgo debug: " << optFibre << " to be positioned " << optFibreX.size() << " times at");
  for (i=0; i<optFibreX.size(); i++)
    DCOUT('A', "\t" << i << "\tx = " << optFibreX[i] << "\tz = " << optFibreZ[i]);

  sideClamp1   = sArgs["SideClamp1Name"];  
  sideClampX   = vArgs["SideClampX"];  
  sideClamp1DZ = vArgs["SideClamp1DZ"];
  DCOUT('A', "DDTOBRodAlgo debug: " << sideClamp1 << " to be positioned " << sideClampX.size() << " times at");
  for (i=0; i<sideClampX.size(); i++)
    DCOUT('A', "\t" << i << "\tx = " << sideClampX[i] << "\tdz = " << sideClamp1DZ[i]);

  sideClamp2   = sArgs["SideClamp2Name"];  
  sideClamp2DZ = vArgs["SideClamp2DZ"];
  DCOUT('A', "DDTOBRodAlgo debug: " << sideClamp2 << " to be positioned " << sideClampX.size() << " times at");
  for (i=0; i<sideClampX.size(); i++)
    DCOUT('A', "\t" << i << "\tx = " << sideClampX[i] << "\tdz = " << sideClamp2DZ[i]);

  moduleRot    = vsArgs["ModuleRot"];   
  module       = sArgs["ModuleName"]; 
  moduleY      = vArgs["ModuleY"];     
  moduleZ      = vArgs["ModuleZ"];
  DCOUT('A', "DDTOBRodAlgo debug:\t" << module << " positioned " << moduleRot.size() << " times");
  for (i=0; i<moduleRot.size(); i++) 
    DCOUT('A', "\tRotation " << moduleRot[i] << "\ty = " << moduleY[i] << "\tz = " << moduleZ[i]);

  connect      = vsArgs["ICCName"];
  connectY     = vArgs["ICCY"];    
  connectZ     = vArgs["ICCZ"];    
  DCOUT('A', "DDTOBRodAlgo debug:\t" << connect.size() << " ICC positioned with no rotation");
  for (i=0; i<connect.size(); i++) 
    DCOUT('A', "\t" << connect[i] << "\ty = " << connectY[i] << "\tz = " << connectZ[i]);
}

void DDTOBRodAlgo::execute() {
  
  DCOUT('a', "==>> Constructing DDTOBRodAlgo...");
  DDName rodName = parent().name();
  DDName centName(DDSplit(central).first, DDSplit(central).second);
  unsigned int i, j;

  // Side Rods
  for (i=0; i<sideRod.size(); i++) {
    for (j=0; j<sideRodX.size(); j++) {
      DDTranslation r(sideRodX[j], sideRodY[i], sideRodZ[i]);
      DDName child(DDSplit(sideRod[i]).first, DDSplit(sideRod[i]).second);
      DDpos(child, rodName, j+1, r, DDRotation());
      DCOUT('a', "DDTOBRodAlgo test: "  << child << " number " << j+1 << " positioned in " << rodName << " at " << r << " with no rotation");
    }
  }

  // Clamps
  for (i=0; i<clampX.size(); i++) {
    DDTranslation r(clampX[i], 0, shift+clampZ[i]);
    DDName child(DDSplit(clamp).first, DDSplit(clamp).second);
    DDpos(child, rodName, i+1, r, DDRotation());
    DCOUT('a', "DDTOBRodAlgo test: " << child << " number " << i+1 << " positioned in " << rodName << " at " << r << " with no rotation");
  }

  // Side Cooling tubes
  for (i=0; i<sideCoolX.size(); i++) {
    DDTranslation r(sideCoolX[i], 0, shift+sideCoolZ[i]);
    DDName child(DDSplit(sideCool).first, DDSplit(sideCool).second);
    DDpos(child, rodName, i+1, r, DDRotation());
    DCOUT('a', "DDTOBRodAlgo test: " << child << " number " << i+1 << " positioned in " << rodName << " at " << r << " with no rotation");
  }

  // Optical Fibres
  for (i=0; i<optFibreX.size(); i++) {
    DDTranslation r(optFibreX[i], 0, shift+optFibreZ[i]);
    DDName child(DDSplit(optFibre).first, DDSplit(optFibre).second);
    DDpos(child, rodName, i+1, r, DDRotation());
    DCOUT('a', "DDTOBRodAlgo test: " << child << " number " << i+1 << " positioned in " << rodName << " at " << r << " with no rotation");
  }

  // Side Clamps
  for (i=0; i<sideClamp1DZ.size(); i++) {
    int j = i/2;
    DDTranslation r(sideClampX[i],moduleY[j],shift+moduleZ[j]+sideClamp1DZ[i]);
    DDName child(DDSplit(sideClamp1).first, DDSplit(sideClamp1).second);
    DDpos(child, rodName, i+1, r, DDRotation());
    DCOUT('a', "DDTOBRodAlgo test: " << child << " number " << i+1 << " positioned in " << rodName << " at " << r << " with no rotation");
  }
  for (i=0; i<sideClamp2DZ.size(); i++) {
    int j = i/2;
    DDTranslation r(sideClampX[i],moduleY[j],shift+moduleZ[j]+sideClamp2DZ[i]);
    DDName child(DDSplit(sideClamp2).first, DDSplit(sideClamp2).second);
    DDpos(child, rodName, i+1, r, DDRotation());
    DCOUT('a', "DDTOBRodAlgo test: " << child << " number " << i+1 << " positioned in " << rodName << " at " << r << " with no rotation");
  }

  // End Rods
  for (i=0; i<endRod1Y.size(); i++) {
    DDTranslation r(0, endRod1Y[i], shift+endRod1Z[i]);
    DDName child(DDSplit(endRod1).first, DDSplit(endRod1).second);
    DDpos(child, centName, i+1, r, DDRotation());
    DCOUT('a', "DDTOBRodAlgo test: " << child << " number " << i+1 << " positioned in " << centName << " at " << r << " with no rotation");
  }
  DDTranslation r1(0, endRod2Y, shift+endRod2Z);
  DDName child1(DDSplit(endRod2).first, DDSplit(endRod2).second);
  DDpos(child1, centName, 1, r1, DDRotation());
  DCOUT('a', "DDTOBRodAlgo test: " << child1 << " number 1 positioned in " << centName << " at " << r1 << " with no rotation");

  // End cooling tubes
  DDTranslation r2(0, 0, shift+endCoolZ);
  string rotstr = DDSplit(endCoolRot).first;
  string rotns  = DDSplit(endCoolRot).second;
  DDRotation rot2(DDName(rotstr,rotns));
  DDName child2(DDSplit(endCool).first, DDSplit(endCool).second);
  DDpos(child2, centName, 1, r2, rot2);
  DCOUT('a', "DDTOBRodAlgo test: " << child2 << " number 1 positioned in " << centName << " at " << r2 << " with " << rot2);

  //Mother cable
  DDTranslation r3(0, 0, shift+cableZ);
  DDName child3(DDSplit(cable).first, DDSplit(cable).second);
  DDpos(child3, centName, 1, r3, DDRotation());
  DCOUT('a', "DDTOBRodAlgo test: " << child3 << " number 1 positioned in " << centName << " at " << r3 << " with no rotation");

  //Modules
  for (i=0; i<moduleRot.size(); i++) {
    DDTranslation r(0, moduleY[i], shift+moduleZ[i]);
    rotstr = DDSplit(moduleRot[i]).first;
    DDRotation rot;
    if (rotstr != "NULL") {
      rotns  = DDSplit(moduleRot[i]).second;
      rot = DDRotation(DDName(rotstr, rotns));
    }
    DDName child(DDSplit(module).first, DDSplit(module).second);
    DDpos(child, centName, i+1, r, rot);
    DCOUT('a', "DDTOBRodAlgo test: " << child << " number " << i+1 << " positioned in " << centName << " at " << r << " with " << rot);
  }

  //Connectors (ICC, CCUM, ...)
  for (i=0; i<connect.size(); i++) {
    DDTranslation r(0, connectY[i], shift+connectZ[i]);
    DDName child(DDSplit(connect[i]).first, DDSplit(connect[i]).second);
    DDpos(child, centName, i+1, r, DDRotation());
    DCOUT('a', "DDTOBRodAlgo test: " << child << " number " << i+1 << " positioned in " << centName << " at " << r << " with no rotation");
  }

  DCOUT('a', "<<== End of DDTOBRodAlgo construction ...");
}