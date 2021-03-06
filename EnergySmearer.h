#ifndef __ENERGYSMEARER__
#define __ENERGYSMEARER__

#include "BaseSmearer.h"
#include <string>
#include <map>
#include <utility>
#include "TFile.h"
#include "TRandom3.h"
#include "TGraphAsymmErrors.h"

class PhotonReducedInfo;
class TRandom3;

// ------------------------------------------------------------------------------------
class EnergyScaleOffset {
public:
	EnergyScaleOffset(int first, int  last) : firstrun(first), lastrun(last) {};
	bool operator == (int run) const { return run>=firstrun && ( lastrun<0 || run<=lastrun); }; 
	bool operator == (const std::pair<int,int> & runrange) const { return runrange.first==firstrun && runrange.second==lastrun; }; 
	
	int firstrun,  lastrun;
	
	std::map<std::string,float> scale_offset;
	std::map<std::string,float> scale_offset_error;
	
};

class PhotonCategory {
public:
	enum photon_type_t { any=0, shperical=1, gap=2 };
	PhotonCategory(float a, float b, float c, float d, photon_type_t e, std::string f ) : mineta(a) , maxeta(b), minr9(c), maxr9(d), type(e), name(f) {};
	
	bool operator == (const PhotonCategory & rh) const { return rh.mineta == mineta && rh.maxeta == maxeta && rh.minr9 == minr9 && rh.maxr9 == maxr9 && rh.name == name; }
	bool operator == (const std::string & catname) const { return catname == name; }
	///////// bool operator == (const std::pair<std::pair<float,float>,std::pair<float,float> > & photonRange ) const { 
	///////// 	return photonRange.first.first == mineta && photonRange.first.second == maxeta && 
	///////// 		photonRange.second.first == minr9 && photonRange.second.second == maxr9; 
	///////// 
	///////// };
	bool operator == (const std::pair<bool,std::pair<float,float> > & photonCoordinates) const { 
		return ( type == any || ( type == shperical && photonCoordinates.first || type == gap && ! photonCoordinates.first ) ) &&
			photonCoordinates.second.first >= mineta && photonCoordinates.second.first <= maxeta && 
			photonCoordinates.second.second >= minr9 && photonCoordinates.second.second <= maxr9; 
	};
	
	std::string name;
	float mineta, maxeta, minr9, maxr9;
	photon_type_t type;
};

// ------------------------------------------------------------------------------------
class EnergySmearer : public BaseSmearer
{
public:

  struct energySmearingParameters
  {
	  int n_categories;
	  bool byRun;
	  std::string categoryType;
	  std::string parameterSetName;

	  typedef std::vector<PhotonCategory> phoCatVector;
	  typedef std::vector<PhotonCategory>::iterator phoCatVectorIt;
	  typedef std::vector<PhotonCategory>::const_iterator phoCatVectorConstIt;

	  typedef std::vector<EnergyScaleOffset> eScaleVector;
	  typedef std::vector<EnergyScaleOffset>::iterator eScaleVectorIt;
	  typedef std::vector<EnergyScaleOffset>::const_iterator eScaleVectorConstIt;
	  
	  typedef std::map<std::string,float> parameterMap;
	  typedef std::map<std::string,float>::iterator parameterMapIt;
	  typedef std::map<std::string,float>::const_iterator parameterMapConstIt;
	  
	  // Scale offset and smearing error should be espressed as a relative value
	  // Example: scale_offset["EB"]=1.002 , smearing_sigma["EB"]=0.01  
	  
	  std::map<std::string,float> scale_offset;
	  std::map<std::string,float> scale_offset_error;
	  
	  std::map<std::string,float> smearing_sigma;
	  std::map<std::string,float> smearing_sigma_error;
	  
	  phoCatVector photon_categories;
	  eScaleVector scale_offset_byrun;
	  
          std::string efficiency_file;
          // errors on correction will be a fraction of the correction itself   
          float       corrRelErr;
  };
  
  EnergySmearer(EnergySmearer * orig, const std::vector<PhotonCategory> & presel=std::vector<PhotonCategory>());

  EnergySmearer(const energySmearingParameters& par, const std::vector<PhotonCategory> & presel=std::vector<PhotonCategory>());
  virtual ~EnergySmearer();
  
  virtual const std::string & name() const { return name_; };
  
  virtual bool smearPhoton(PhotonReducedInfo &, float & weight, int run, float syst_shift) const;
  float getScaleOffset(int run, const std::string & category) const;

  void name(const std::string & x) { name_ = x; };
  
  void scaleOrSmear(bool x) { scaleOrSmear_=x; }; 

  void doEnergy(bool x) { doEnergy_=x; }; 

  void doRegressionSigma(bool x) { doRegressionSmear_=x; }; 

  void doCorrections(bool x) { doCorrections_=x; }; 
   
  void doEfficiencies(bool x) { doEfficiencies_=x; }; 
  
  void setEffName(std::string x) { effName_ =x; };

  void resetRandom(){rgen_->SetSeed(12345);};

  bool initEfficiency();
  
  void syst_only(bool x) { syst_only_ = x; };

  energySmearingParameters  myParameters_;
  
  std::string photonCategory(PhotonReducedInfo &) const;

 protected:
  bool doEnergy_, scaleOrSmear_, doEfficiencies_, doCorrections_, doRegressionSmear_;
  int baseSeed_;

  std::vector<PhotonCategory> preselCategories_;
  
  double getWeight(double pt, std::string theCategory, float syst_shift) const;
  
  std::string   name_;
  TRandom3     *rgen_;
  std::string   effName_;
  TFile        *theEfficiencyFile_; 
  bool syst_only_;
  std::map<std::string,TGraphAsymmErrors*> smearing_eff_graph_;
};

#endif
