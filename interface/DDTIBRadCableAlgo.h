#ifndef DD_TIBRadCableAlgo_h
#define DD_TIBRadCableAlgo_h

#include <map>
#include <string>
#include <vector>
#include "DetectorDescription/Base/interface/DDTypes.h"
#include "DetectorDescription/Algorithm/interface/DDAlgorithm.h"

class DDTIBRadCableAlgo : public DDAlgorithm {
 public:
  //Constructor and Destructor
  DDTIBRadCableAlgo(); 
  virtual ~DDTIBRadCableAlgo();
  
  void initialize(const DDNumericArguments & nArgs,
		  const DDVectorArguments & vArgs,
		  const DDMapArguments & mArgs,
		  const DDStringArguments & sArgs,
		  const DDStringVectorArguments & vsArgs);

  void execute();

private:

  std::string         idNameSpace;   //Namespace of this and ALL sub-parts

  double              rMin;          //Minimum radius
  double              rMax;          //Maximum radius
  std::vector<double> layRin;        //Radii for inner layer
  double              deltaR;        //DeltaR between inner and outer layer
  double              cylinderT;     //Support cylinder thickness
  double              supportT;      //Support disk thickness
  double              supportDR;     //Extra width along R
  std::string         supportMat;    //Material for support disk
  double              cableT;        //Cable thickness
  std::vector<std::string> cableMat; //Materials for cables
  std::vector<std::string> strucMat; //Materials for open structures
};

#endif