
///////////////////////////////////////////////////////////////////////////////
// File: DDTECModuleAlgo	.cc
// Description: Creation of a TEC Test
///////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <algorithm>

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DetectorDescription/Core/interface/DDPosPart.h"
#include "DetectorDescription/Core/interface/DDLogicalPart.h"
#include "DetectorDescription/Core/interface/DDSolid.h"
#include "DetectorDescription/Core/interface/DDMaterial.h"
#include "DetectorDescription/Core/interface/DDCurrentNamespace.h"
#include "DetectorDescription/Core/interface/DDSplit.h"
#include "Geometry/TrackerCommonData/interface/DDTECModuleAlgo.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"


DDTECModuleAlgo::DDTECModuleAlgo() {
  LogDebug("TECGeom") << "DDTECModuleAlgo info: Creating an instance";
}

DDTECModuleAlgo::~DDTECModuleAlgo() {}

void DDTECModuleAlgo::initialize(const DDNumericArguments & nArgs,
				 const DDVectorArguments & vArgs,
				 const DDMapArguments & ,
				 const DDStringArguments & sArgs,
				 const DDStringVectorArguments & vsArgs) {

  idNameSpace  = DDCurrentNamespace::ns();
  genMat       = sArgs["GeneralMaterial"];

  DDName parentName = parent().name(); 

  LogDebug("TECGeom") << "DDTECModuleAlgo debug: Parent " << parentName 
		      << " NameSpace " << idNameSpace << " General Material "
		      << genMat;

  moduleThick    = nArgs["ModuleThick"];
  detTilt        = nArgs["DetTilt"];
  fullHeight     = nArgs["FullHeight"];
  dlTop          = nArgs["DlTop"];
  dlBottom       = nArgs["DlBottom"];
  dlHybrid       = nArgs["DlHybrid"];
  rPos           = nArgs["RPos"];
  standardRot    = sArgs["StandardRotation"];

  isRing6 = dlHybrid < dlTop;

  LogDebug("TECGeom") << "DDTECModuleAlgo debug: ModuleThick " << moduleThick
					  << " Detector Tilt " << detTilt/deg << " Height "
					  << fullHeight << " dl(Top) " << dlTop << " dl(Bottom) "
					  << dlBottom << " dl(Hybrid) " << dlHybrid
					  << " rPos " << rPos<< " standrad rotation " << standardRot;

  frameWidth     = nArgs["FrameWidth"];
  frameThick     = nArgs["FrameThick"];
  frameOver      = nArgs["FrameOver"];
  LogDebug("TECGeom") << "DDTECModuleAlgo debug: Frame Width " << frameWidth 
		      << " Thickness " << frameThick << " Overlap " 
		      << frameOver;

  topFrameMat    = sArgs["TopFrameMaterial"];
  topFrameHeight = nArgs["TopFrameHeight"];
  topFrameTopWidth= nArgs["TopFrameTopWidth"];
  topFrameBotWidth= nArgs["TopFrameBotWidth"];
  topFrameThick  = nArgs["TopFrameThick"];
  topFrameZ      = nArgs["TopFrameZ"];
  LogDebug("TECGeom") << "DDTECModuleAlgo debug: Top Frame Material " 
		      << topFrameMat << " Height " << topFrameHeight 
		      << " Top Width " << topFrameTopWidth << " Bottom Width "
		      << topFrameTopWidth
		      << " Thickness " << topFrameThick <<" positioned at"
		      << topFrameZ;
  double resizeH =0.96;
  sideFrameMat   = sArgs["SideFrameMaterial"];
  sideFrameThick = nArgs["SideFrameThick"];
  sideFrameLWidth =  nArgs["SideFrameLWidth"]; 
  sideFrameLHeight = resizeH*nArgs["SideFrameLHeight"];
  sideFrameLtheta =  nArgs["SideFrameLtheta"];
  sideFrameRWidth =  nArgs["SideFrameRWidth"]; 
  sideFrameRHeight = resizeH*nArgs["SideFrameRHeight"];
  sideFrameRtheta =  nArgs["SideFrameRtheta"];
  siFrSuppBoxWidth  = vArgs["SiFrSuppBoxWidth"];
  siFrSuppBoxHeight = vArgs["SiFrSuppBoxHeight"];
  siFrSuppBoxYPos = vArgs["SiFrSuppBoxYPos"];
  siFrSuppBoxThick =  nArgs["SiFrSuppBoxThick"]; 
  siFrSuppBoxMat = sArgs["SiFrSuppBoxMaterial"]; 
  sideFrameZ     = nArgs["SideFrameZ"];
  LogDebug("TECGeom") << "DDTECModuleAlgo debug : Side Frame Material " 
		      << sideFrameMat << " Thickness " << sideFrameThick
		      << " left Leg's Width: " << sideFrameLWidth
		      << " left Leg's Height: " << sideFrameLHeight
		      << " left Leg's tilt(theta): " << sideFrameLtheta
		      << " right Leg's Width: " << sideFrameRWidth
		      << " right Leg's Height: " << sideFrameRHeight
		      << " right Leg's tilt(theta): " << sideFrameRtheta
		      << "Supplies Box's Material: " << siFrSuppBoxMat
		      << " positioned at" << sideFrameZ;
  for (int i= 0; i < (int)(siFrSuppBoxWidth.size());i++){
    LogDebug("TECGeom") << " Supplies Box"<<i<<"'s Width: " << siFrSuppBoxWidth[i]
			<< " Supplies Box"<<i<<"'s Height: " << siFrSuppBoxHeight[i]
			<< " Supplies Box"<<i<<"'s y Position: " << siFrSuppBoxYPos[i];
  }
  waferMat       = sArgs["WaferMaterial"];
  sideWidthTop   = nArgs["SideWidthTop"];
  sideWidthBottom= nArgs["SideWidthBottom"];
  waferRot       = sArgs["WaferRotation"];
  waferPosition  = nArgs["WaferPosition"];
  LogDebug("TECGeom") << "DDTECModuleAlgo debug: Wafer Material " 
		      << waferMat << " Side Width Top" << sideWidthTop
		      << " Side Width Bottom" << sideWidthBottom
		      << " and positioned at "<<waferPosition
		      << " positioned with rotation"	<< " matrix:"
		      << waferRot;

  activeMat      = sArgs["ActiveMaterial"];
  activeHeight   = nArgs["ActiveHeight"];
  activeThick    = nArgs["ActiveThick"];
  activeRot      = sArgs["ActiveRotation"];
  activeZ        = nArgs["ActiveZ"];
  LogDebug("TECGeom") << "DDTECModuleAlgo debug: Active Material " 
		      << activeMat << " Height " << activeHeight 
		      << " rotated by " << activeRot << " Thickness/Z"
		      << activeThick << "/"	<< activeZ;

  hybridMat      = sArgs["HybridMaterial"];
  hybridHeight   = nArgs["HybridHeight"];
  hybridWidth    = nArgs["HybridWidth"];
  hybridThick    = nArgs["HybridThick"];
  hybridZ        = nArgs["HybridZ"];
  LogDebug("TECGeom") << "DDTECModuleAlgo debug: Hybrid Material " 
		      << hybridMat << " Height " << hybridHeight 
		      << " Width " << hybridWidth << " Thickness " 
		      << hybridThick << " Z"  << hybridZ;

  pitchMat       = sArgs["PitchMaterial"];
  pitchHeight    = nArgs["PitchHeight"];
  pitchThick     = nArgs["PitchThick"];
  pitchWidth     = nArgs["PitchWidth"];
  pitchZ         = nArgs["PitchZ"];
  pitchRot       = sArgs["PitchRotation"];
  LogDebug("TECGeom") << "DDTECModuleAlgo debug: Pitch Adapter Material " 
		      << pitchMat << " Height " << pitchHeight 
		      << " Thickness " << pitchThick << " position with "
		      << " rotation " << pitchRot << " at Z" << pitchZ;

  bridgeMat      = sArgs["BridgeMaterial"];
  bridgeWidth    = nArgs["BridgeWidth"];
  bridgeThick    = nArgs["BridgeThick"];
  bridgeHeight   = nArgs["BridgeHeight"];
  bridgeSep      = nArgs["BridgeSeparation"];
  LogDebug("TECGeom") << "DDTECModuleAlgo debug: Bridge Material " 
		      << bridgeMat << " Width " << bridgeWidth 
		      << " Thickness " << bridgeThick << " Height " 
		      << bridgeHeight << " Separation "<< bridgeSep;

  siReenforceWidth  = vArgs["SiReenforcementWidth"];
  siReenforceHeight = vArgs["SiReenforcementHeight"];
  siReenforceYPos =   vArgs["SiReenforcementPosY"];
  siReenforceThick =  nArgs["SiReenforcementThick"]; 
  siReenforceMat   =  sArgs["SiReenforcementMaterial"];
 
  LogDebug("TECGeom") << "FALTBOOT DDTECModuleAlgo debug : Si-Reenforcement Material " 
		      << sideFrameMat << " Thickness " << siReenforceThick;
    
  for (int i= 0; i < (int)(siReenforceWidth.size());i++){
    LogDebug("TECGeom") << " SiReenforcement"<<i<<"'s Width: " <<     siReenforceWidth[i]
			<< " SiReenforcement"<<i<<"'s Height: " <<    siReenforceHeight[i]
			<< " SiReenforcement"<<i<<"'s y Position: " <<siReenforceYPos[i];
  }

    noOverlapShift = nArgs["NoOverlapShift"];
  //Everything that is normal/stereo specific comes here
  isStereo = (int)nArgs["isStereo"] == 1;
  if(!isStereo){
    LogDebug("TECGeom") << "This is a normal module!"; 
  }
  else{
    LogDebug("TECGeom") << "This is a stereo module!"; 
    topFrame2LHeight = nArgs["TopFrame2LHeight"];
    topFrame2RHeight = nArgs["TopFrame2RHeight"];
    topFrame2Width   = nArgs["TopFrame2Width"];

    LogDebug("TECGeom") << "DDTECModuleAlgo debug: stereo Top Frame 2nd Part left Heigt " 
			<< topFrame2LHeight << " right Height " << topFrame2RHeight 
      		        << " Width " << topFrame2Width ;

    sideFrameLWidthLow =  nArgs["SideFrameLWidthLow"]; 
    sideFrameRWidthLow =  nArgs["SideFrameRWidthLow"]; 

    LogDebug("TECGeom") << " left Leg's lower Width: " << sideFrameLWidthLow
			<< " right Leg's lower Width: " << sideFrameRWidthLow;

	// posCorrectionR =  nArgs["PosCorrectionR"]; 
    //LogDebug("TECGeom") << "Stereo Module Position Correction with R = " << posCorrectionR;
  }
}

void DDTECModuleAlgo::doPos(DDLogicalPart toPos, DDLogicalPart mother, 
			    int copyNr, double x, double y, double z, 
			    std::string rotName) {

  DDTranslation tran(z, x, y);
  DDRotation rot;
  std::string rotstr = DDSplit(rotName).first;
  std::string rotns; 
  if (rotstr != "NULL") {
    rotns = DDSplit(rotName).second;
    rot   = DDRotation(DDName(rotstr, rotns));
  } else {
    rot = DDRotation();
  }
	
  DDpos (toPos, mother, copyNr, tran, rot);
  LogDebug("TECGeom") << "DDTECModuleAlgo test: " << toPos.name()
		      << " positioned in "<< mother.name() 
		      << " at " << tran  << " with " << rot;
}

void DDTECModuleAlgo::doPos(DDLogicalPart toPos, double x, double y, double z,
			    std::string rotName) {
  int           copyNr = 1;
  if (isStereo) copyNr = 2;

  // This has to be done so that the Mother coordinate System of a Tub resembles 
  // the coordinate System of a Trap or Box.
  z += rPos;
  if (rotName == "NULL") rotName = standardRot;

  doPos(toPos,parent(),copyNr,x,y,z,rotName);
}

void DDTECModuleAlgo::execute() {

  LogDebug("TECGeom") << "==>> Constructing DDTECModuleAlgo...";
  //declarations
  double tmp;
  double dxdif, dzdif;
  double dxbot, dxtop, topfr;
  //positions
  double xpos, ypos, zpos;
  //dimensons
  double bl1, bl2;
  double h1;
  double dx, dy, dz;
  double thet;
  //names
  std::string idName;
  std::string name;
  std::string tag("Rphi");
  if (isStereo) tag = "Stereo";
  char buf[5]; //for string operations

  DDName  parentName = parent().name(); 
  idName = DDSplit(parentName).first;
  LogDebug("TECGeom") << "==>> " << idName << " parent " << parentName << " namespace " << idNameSpace;
  DDSolid solid;

  //set global parameters
  DDName matname(DDSplit(genMat).first, DDSplit(genMat).second);
  DDMaterial matter(matname);
  dzdif = fullHeight + topFrameHeight;
  if(isStereo) dzdif += 0.5*(topFrame2LHeight+topFrame2RHeight);
  
  dxbot = 0.5*dlBottom + frameWidth - frameOver;
  dxtop = 0.5*dlHybrid + frameWidth - frameOver;
  topfr = 0.5*dlBottom * sin(detTilt);
  if(isRing6){
	dxbot = dxtop;
	dxtop = 0.5*dlTop    + frameWidth - frameOver;
	topfr = 0.5*dlTop    * sin(detTilt);
  }
  dxdif = dxtop - dxbot;

  //Frame Sides
  // left Frame
  name    = idName + "SideFrameLeft";
  matname =  DDName(DDSplit(sideFrameMat).first, DDSplit(sideFrameMat).second);
  matter  = DDMaterial(matname);

  h1 = 0.5 * sideFrameThick;
  dz = 0.5 * sideFrameLHeight;
  bl1 = bl2 = 0.5 * sideFrameLWidth;
  thet = sideFrameLtheta;
  //for stereo modules
  if(isStereo)  bl1 = 0.5 * sideFrameLWidthLow;
  solid = DDSolidFactory::trap(DDName(name,idNameSpace), dz, thet, 0, h1, bl1, 
			       bl1, 0, h1, bl2, bl2, 0);
  LogDebug("TECGeom") << "DDTECModuleAlgo test:\t" << solid.name() 
		      << " Trap made of " << matname << " of dimensions "
		      << dz << ",  "<<thet<<", 0, " << h1 << ", " << bl1 << ", "
		      << bl1 << ", 0, " << h1 << ", " << bl2 << ", " << bl2
		      << ", 0";
  DDLogicalPart sideFrameLeft(solid.ddname(), matter, solid);
  //translate
  xpos = - 0.5*topFrameBotWidth +bl1+ tan(fabs(thet)) * dz;
  ypos = sideFrameZ;// + noOverlapShift;
  zpos = 0.5 * (-waferPosition + fullHeight) -dz + pitchHeight + hybridHeight - topFrameHeight;
  //flip ring 6
  if (isRing6){
    zpos *= -1;
    xpos -= 2*tan(fabs(thet)) * dz; // because of the flip the tan(..) to be in the other direction
  }
  //the stereo modules are on the back of the normal ones...
  if(isStereo) {
    xpos = -xpos;
    zpos += -topFrame2LHeight-tan(detTilt)*(fabs(xpos)-0.5*topFrame2Width)- noOverlapShift;
    //   ypos -= 2* noOverlapShift;
  }
  //position
  doPos(sideFrameLeft,xpos,ypos,zpos,waferRot);

  //right Frame
  name    = idName + "SideFrameRight";
  matname = DDName(DDSplit(sideFrameMat).first, DDSplit(sideFrameMat).second);
  matter  = DDMaterial(matname);

  h1 = 0.5 * sideFrameThick;
  dz = 0.5 * sideFrameRHeight;
  bl1 = bl2 = 0.5 * sideFrameRWidth;
  thet = sideFrameRtheta;
  if(isStereo) bl1 = 0.5 * sideFrameRWidthLow;
  solid = DDSolidFactory::trap(DDName(name,idNameSpace), dz, thet, 0, h1, bl1, 
			       bl1, 0, h1, bl2, bl2, 0);
  LogDebug("TECGeom") << "DDTECModuleAlgo test:\t" << solid.name() 
		      << " Trap made of " << matname << " of dimensions "
		      << dz << ", "<<thet<<", 0, " << h1 << ", " << bl1 << ", "
		      << bl1 << ", 0, " << h1 << ", " << bl2 << ", " << bl2
		      << ", 0";
  DDLogicalPart sideFrameRight(solid.ddname(), matter, solid);
  //translate
  xpos =  0.5*topFrameBotWidth -bl1- tan(fabs(thet)) * dz;
  ypos = sideFrameZ;
  zpos = 0.5 * (-waferPosition + fullHeight) -dz + pitchHeight + hybridHeight - topFrameHeight;        
  if (isRing6){
    zpos *= -1;
    xpos += 2*tan(fabs(thet)) * dz; // because of the flip the tan(..) has to be in the other direction
  }
  if(isStereo){
    xpos = -xpos;
    zpos += -topFrame2RHeight+tan(detTilt)*(fabs(xpos)-0.5*topFrame2Width)-2*noOverlapShift;
  }
  //position it
  doPos(sideFrameRight,xpos,ypos,zpos,waferRot);


  //Supplies Box(es)
  for (int i= 0; i < (int)(siFrSuppBoxWidth.size());i++){
    sprintf(buf,"%i",i);
    name    = idName + "SuppliesBox"+buf;
    matname = DDName(DDSplit(siFrSuppBoxMat).first, DDSplit(siFrSuppBoxMat).second);
    matter  = DDMaterial(matname);
    
    h1 = 0.5 * siFrSuppBoxThick;
    dz = 0.5 * siFrSuppBoxHeight[i];
    bl1 = bl2 = 0.5 * siFrSuppBoxWidth[i];
    thet = sideFrameRtheta;
    if(isStereo) thet = -(atan((sideFrameRWidthLow-sideFrameRWidth)/(2*sideFrameRHeight)+tan(fabs(thet))));
                   // ^-- this calculates the lower left angel of the tipped trapezoid, which is the SideFframe...

    solid = DDSolidFactory::trap(DDName(name,idNameSpace), dz, thet, 0, h1, bl1, 
				 bl1, 0, h1, bl2, bl2, 0);
    LogDebug("TECGeom") << "DDTECModuleAlgo test:\t" << solid.name() 
			<< " Trap made of " << matname << " of dimensions "
			<< dz << ", 0, 0, " << h1 << ", " << bl1 << ", "
			<< bl1 << ", 0, " << h1 << ", " << bl2 << ", " << bl2
			<< ", 0";
    DDLogicalPart siFrSuppBox(solid.ddname(), matter, solid);
    //translate
    xpos =  0.5*topFrameBotWidth  -sideFrameRWidth - bl1-fabs(tan(thet) * siFrSuppBoxYPos[i])-2*noOverlapShift;
    ypos = sideFrameZ*(0.5+(siFrSuppBoxThick/sideFrameThick)); //via * so I do not have to worry about the sign of sideFrameZ
    zpos = 0.5* (-waferPosition +fullHeight) -siFrSuppBoxYPos[i];        
	if (isRing6){ 
	  xpos += 2*fabs(tan(thet))*  siFrSuppBoxYPos[i]; // the flipped issue again
	  zpos *= -1;
	}
	if(isStereo){ 
	  xpos += sin(detTilt)* siFrSuppBoxYPos[i];
	  zpos = 0.5* (-waferPosition +fullHeight)-topFrame2RHeight-sin(detTilt)*(sideFrameRWidth+bl1)-siFrSuppBoxYPos[i];
	  xpos = -xpos;
	}
	//position it;
	doPos(siFrSuppBox,xpos,ypos,zpos,waferRot);
  }
  //The Hybrid
  name    = idName + "Hybrid";
  matname = DDName(DDSplit(hybridMat).first, DDSplit(hybridMat).second);
  matter  = DDMaterial(matname);
  dx = 0.5 * hybridWidth;
  dy = 0.5 * hybridThick;
  dz        = 0.5 * hybridHeight;
  solid = DDSolidFactory::box(DDName(name, idNameSpace), dx, dy, dz);
  LogDebug("TECGeom") << "DDTECModuleAlgo test:\t" << solid.name() 
					  << " Box made of " << matname << " of dimensions "
					  << dx << ", " << dy << ", " << dz;
  DDLogicalPart hybrid(solid.ddname(), matter, solid);

  ypos = hybridZ;
  zpos = 0.5 * (-waferPosition + fullHeight + hybridHeight)+pitchHeight;
  if (isRing6)	zpos *=-1;
  //position it
  doPos(hybrid,0,ypos,zpos,"NULL");  

  // Wafer
  name    = idName + tag +"Wafer";
  matname = DDName(DDSplit(waferMat).first, DDSplit(waferMat).second);
  matter  = DDMaterial(matname);
  bl1     = 0.5 * dlBottom;
  bl2     = 0.5 * dlTop;
  h1      = 0.5 * activeThick;
  dz      = 0.5 * fullHeight;
  solid = DDSolidFactory::trap(DDName(name,idNameSpace), dz, 0, 0, h1, bl1, 
							   bl1, 0, h1, bl2, bl2, 0);
  LogDebug("TECGeom") << "DDTECModuleAlgo test:\t" << solid.name()
			<< " Trap made of " << matname << " of dimensions "
			<< dz << ", 0, 0, " << h1 << ", " << bl1 << ", "
			<< bl1 << ", 0, " << h1 << ", " << bl2 << ", "
			<< bl2 << ", 0";
  DDLogicalPart wafer(solid.ddname(), matter, solid);
  
  ypos = activeZ;
  zpos =-0.5 * waferPosition;// former and incorrect topFrameHeight;
  if (isRing6) zpos *= -1;
  
  doPos(wafer,0,ypos,zpos,waferRot);
  
  // Active
  name    = idName + tag +"Active";
  matname = DDName(DDSplit(activeMat).first, DDSplit(activeMat).second);
  matter  = DDMaterial(matname);
  bl1    -= sideWidthBottom;
  bl2    -= sideWidthTop;
  dz      = 0.5 * activeThick;
  h1      = 0.5 * activeHeight;
  if (isRing6) { //switch bl1 <->bl2
	tmp = bl2;	bl2 =bl1;	bl1 = tmp;
  }
  solid = DDSolidFactory::trap(DDName(name,idNameSpace), dz, 0, 0, h1, bl2, 
							   bl1, 0, h1, bl2, bl1, 0);
  LogDebug("TECGeom") << "DDTECModuleAlgo test:\t" << solid.name() 
					  << " Trap made of " << matname << " of dimensions "
					  << dz << ", 0, 0, " << h1 << ", " << bl2 << ", "
					  << bl1 << ", 0, " << h1 << ", " << bl2 << ", "
					  << bl1 << ", 0";
  DDLogicalPart active(solid.ddname(), matter, solid);
  doPos(active, wafer, 1, 0,0,0, activeRot);
  
  //Pitch Adapter
  name    = idName + "PA";
  matname = DDName(DDSplit(pitchMat).first, DDSplit(pitchMat).second);
  matter  = DDMaterial(matname);
  
  if (!isStereo) {
	dx      = 0.5 * pitchWidth;
	dy      = 0.5 * pitchThick;
	dz      = 0.5 * pitchHeight;
	solid   = DDSolidFactory::box(DDName(name, idNameSpace), dx, dy, dz);
	LogDebug("TECGeom") << "DDTECModuleAlgo test:\t" << solid.name() 
						<< " Box made of " << matname <<" of dimensions "
						<< dx << ", " << dy << ", " << dz;
  } else {
	dz      = 0.5 * pitchWidth;    
	h1      = 0.5 * pitchThick;
	bl1     = 0.5 * pitchHeight + 0.5 * dz * sin(detTilt);
	bl2     = 0.5 * pitchHeight - 0.5 * dz * sin(detTilt);
	double thet = atan((bl1-bl2)/(2.*dz));
	solid   = DDSolidFactory::trap(DDName(name,idNameSpace), dz, thet, 0, h1,
								   bl1, bl1, 0, h1, bl2, bl2, 0);
	LogDebug("TECGeom") << "DDTECModuleAlgo test:\t" << solid.name()
						<< " Trap made of " << matname 
						<< " of dimensions " << dz << ", " << thet/deg
						<< ", 0, " << h1 << ", " << bl1 << ", " << bl1
						<< ", 0, " << h1 << ", " << bl2 << ", " << bl2
						<< ", 0";
    }
  xpos = 0;
  ypos = pitchZ;
  zpos = 0.5 * (-waferPosition + fullHeight + pitchHeight);
  if (isRing6) zpos *= -1;
  if(isStereo){
    //ypos += 2*noOverlapShift;
    xpos    = 0.5 * fullHeight * sin(detTilt);
    //    zpos    -= 0.5*(  topFrame2LHeight+ topFrame2RHeight); 
  }
  
  DDLogicalPart pa(solid.ddname(), matter, solid);
  if(isStereo)doPos(pa, xpos, ypos,zpos, pitchRot);
  else        doPos(pa, xpos, ypos,zpos, "NULL");
  //Top of the frame
  name = idName + "TopFrame";
  matname = DDName(DDSplit(topFrameMat).first, DDSplit(topFrameMat).second);
  matter  = DDMaterial(matname);
  
  h1 = 0.5 * topFrameThick;
  dz = 0.5 * topFrameHeight;
  bl1 = 0.5 * topFrameBotWidth;
  bl2 = 0.5 * topFrameTopWidth;
  if (isRing6) {    // ring 6 faces the other way!
	bl1 = 0.5 * topFrameTopWidth;
	bl2 = 0.5 * topFrameBotWidth;
  }
  
  solid = DDSolidFactory::trap(DDName(name,idNameSpace), dz, 0, 0, h1, bl1, 
							   bl1,0, h1, bl2, bl2, 0);
  LogDebug("TECGeom") << "DDTECModuleAlgo test:\t" << solid.name() 
					  << " Trap made of " << matname << " of dimensions " 
					  << dz << ", 0, 0, " << h1 << ", " << bl1 << ", "  
					  << bl1 << ", 0, " << h1 << ", " << bl2 << ", " << bl2
					  << ", 0";
  DDLogicalPart topFrame(solid.ddname(), matter, solid);
  
  if(isStereo){ 
	name = idName + "TopFrame2";
      //additional object to build the not trapzoid geometry of the stereo topframes
	dz      = 0.5 * topFrame2Width;    
	h1      = 0.5 * topFrameThick;
	bl1     = 0.5 * topFrame2LHeight;
	bl2     = 0.5 * topFrame2RHeight;
	double thet = atan((bl1-bl2)/(2.*dz));
	
	solid   = DDSolidFactory::trap(DDName(name,idNameSpace), dz, thet, 0, h1,
								   bl1, bl1, 0, h1, bl2, bl2, 0);
	LogDebug("TECGeom") << "DDTECModuleAlgo test:\t" << solid.name()
						<< " Trap made of " << matname 
						<< " of dimensions " << dz << ", " << thet/deg
						<< ", 0, " << h1 << ", " << bl1 << ", " << bl1
						<< ", 0, " << h1 << ", " << bl2 << ", " << bl2
						<< ", 0";
  }
  
  // Position the topframe
  ypos = topFrameZ;
  zpos = 0.5 * (-waferPosition + fullHeight - topFrameHeight)+ pitchHeight + hybridHeight;
  if(isRing6){
	zpos *=-1;
  }

  doPos(topFrame, 0,ypos,zpos,"NULL");
  if(isStereo){
	//create
	DDLogicalPart topFrame2(solid.ddname(), matter, solid);
	zpos -= 0.5*(topFrameHeight + 0.5*(topFrame2LHeight+topFrame2RHeight));
	doPos(topFrame2, 0,ypos,zpos,pitchRot);
  }
  
  //Si - Reencorcement
  for (int i= 0; i < (int)(siReenforceWidth.size());i++){
	char buf[5];
	sprintf(buf,"%i",i);
	name    = idName + "SiReenforce"+buf;
    matname = DDName(DDSplit(siReenforceMat).first, DDSplit(siReenforceMat).second);
    matter  = DDMaterial(matname);
    
    h1 = 0.5 * siReenforceThick;
    dz = 0.5 * siReenforceHeight[i];
    bl1 = bl2 = 0.5 * siReenforceWidth[i];
    
    solid = DDSolidFactory::trap(DDName(name,idNameSpace), dz, 0, 0, h1, bl1, 
								 bl1, 0, h1, bl2, bl2, 0);
    LogDebug("TECGeom") << "DDTECModuleAlgo test:\t" << solid.name() 
						<< " Trap made of " << matname << " of dimensions "
						<< dz << ", 0, 0, " << h1 << ", " << bl1 << ", "
						<< bl1 << ", 0, " << h1 << ", " << bl2 << ", " << bl2
						<< ", 0";
    DDLogicalPart siReenforce(solid.ddname(), matter, solid);
    //translate
    xpos =0 ;
    ypos =  sideFrameZ;
    //(sideFrameZ)*(0.5- noOverlapShift/fabs(sideFrameZ) +(siReenforceThick/sideFrameThick)); //via * so I do not have to worry about the sign of sideFrameZ
    zpos = 0.5 * (-waferPosition + fullHeight- siReenforceHeight[i]) + pitchHeight + hybridHeight - topFrameHeight -siReenforceYPos[i];
	
    if (isRing6)  zpos *= -1;
    if(isStereo){ 
	  xpos = (-siReenforceYPos[i]+0.5*fullHeight)*sin(detTilt);
	  // zpos = 0.5 * (waferPosition + fullHeight) - topFrameHeight-0.5*(topFrame2RHeight+topFrame2LHeight)-siReenforceYPos[i]- 0.5 * siReenforceHeight[i]- noOverlapShift;
	  thet = detTilt;
	  if(topFrame2RHeight > topFrame2LHeight) thet *= -1;
	    zpos -= topFrame2RHeight + sin(thet)*(sideFrameRWidth + 0.5*dlTop);
    }
    doPos(siReenforce,xpos,ypos,zpos,waferRot);
  }

  //Bridge 
  name    = idName + "Bridge";
  matname = DDName(DDSplit(bridgeMat).first, DDSplit(bridgeMat).second);
  matter  = DDMaterial(matname);
  bl2     = 0.5*bridgeSep + bridgeWidth;
  bl1     = bl2 - bridgeHeight * dxdif / dzdif;
  h1      = 0.5 * bridgeThick;
  dz      = 0.5 * bridgeHeight;
  solid = DDSolidFactory::trap(DDName(name,idNameSpace), dz, 0, 0, h1, bl1, 
			       bl1, 0, h1, bl2, bl2, 0);
  LogDebug("TECGeom") << "DDTECModuleAlgo test:\t" << solid.name()
		      << " Trap made of " << matname << " of dimensions "
		      << dz << ", 0, 0, " << h1 << ", " << bl1 << ", "
		      << bl1 << ", 0, " << h1 << ", " << bl2 << ", " << bl2
		      << ", 0";
  DDLogicalPart bridge(solid.ddname(), matter, solid);

  name    = idName + "BridgeGap";
  matname = DDName(DDSplit(genMat).first, DDSplit(genMat).second);
  matter  = DDMaterial(matname);
  bl1     = 0.5*bridgeSep;
  solid = DDSolidFactory::box(DDName(name,idNameSpace), bl1, h1, dz);
  LogDebug("TECGeom") << "DDTECModuleAlgo test:\t" << solid.name() 
		      << " Box made of " << matname << " of dimensions "
		      << bl1 << ", " << h1 << ", " << dz;
  DDLogicalPart bridgeGap(solid.ddname(), matter, solid);
  DDpos (bridgeGap, bridge, 1, DDTranslation(0.0, 0.0, 0.0), DDRotation());
  LogDebug("TECGeom") << "DDTECModuleAlgo test: " << bridgeGap.name() 
		      << " number 1 positioned in " << bridge.name()
		      << " at (0,0,0) with no rotation";

  LogDebug("TECGeom") << "<<== End of DDTECModuleAlgo construction ...";
}
