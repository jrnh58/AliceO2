// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file HwClusterer.h
/// \brief Class for TPC HW cluster finding
/// \author Sebastian Klewin
#ifndef ALICEO2_TPC_HWClusterer_H_
#define ALICEO2_TPC_HWClusterer_H_

#include "TPCReconstruction/Clusterer.h"
#include "TPCReconstruction/HwCluster.h"
#include "TPCBase/CalDet.h" 

#include <vector>

namespace o2{
namespace TPC {
    
class ClusterContainer;
class ClustererTask;
class HwClusterFinder;
class HwCluster;
class Digit;

/// \class HwClusterer
/// \brief Class for TPC HW cluster finding
class HwClusterer : public Clusterer {
  public:
    enum class Processing : int { Sequential, Parallel};

    /// Constructor
    /// \param output is pointer to vector to be filled with clusters
    /// \param processingType parallel or sequential
    /// \param globalTime value of first timebin
    /// \param cru Number of CRUs to process
    /// \param minQDiff Min charge differece 
    /// \param assignChargeUnique Avoid using same charge for multiple nearby clusters
    /// \param enableNoiseSim Enables the Noise simulation for empty pads (noise object has to be set)
    /// \param enablePedestalSubtraction Enables the Pedestal subtraction (pedestal object has to be set)
    /// \param padsPerCF Pads per cluster finder
    /// \param timebinsPerCF Timebins per cluster finder
    /// \param cfPerRow Number of cluster finder in each row
    HwClusterer(
	std::vector<o2::TPC::HwCluster> *output,
	Processing processingType = Processing::Parallel,
        int globalTime = 0, 
        int cruMin = 0,
        int cruMax = 360, 
        float minQDiff = 0,
        bool assignChargeUnique = false,
        bool enableNoiseSim = true, 
        bool enablePedestalSubtraction = true, 
        int padsPerCF = 8, 
        int timebinsPerCF = 8, 
        int cfPerRow = 0);
    
    /// Destructor
    ~HwClusterer() override;
    
    // Should this really be a public member?
    // Maybe better to just call by process
    void Init() override;
    
    /// Steer conversion of points to digits
    /// @param digits Container with TPC digits
    /// @return Container with clusters
    void Process(std::vector<o2::TPC::Digit> const &digits) override;
    void Process(std::vector<std::unique_ptr<Digit>>& digits) override;

    void setProcessingType(Processing processing)    { mProcessingType = processing; };   

    void setNoiseObject(CalDet<float>* noiseObject) { mNoiseObject = noiseObject; };
    void setPedestalObject(CalDet<float>* pedestalObject) { mPedestalObject = pedestalObject; };

    /// Switch for triggered / continuous readout
    /// \param isContinuous - false for triggered readout, true for continuous readout
    void setContinuousReadout(bool isContinuous) { mIsContinuousReadout = isContinuous; };

    void setCRUMin(int cru) { mCRUMin = cru; };
    void setCRUMax(int cru) { mCRUMax = cru; };
    
  private:
    // To be done
    /* BoxClusterer(const BoxClusterer &); */
    /* BoxClusterer &operator=(const BoxClusterer &); */

    struct CfConfig {
      int iCRU;
      int iMaxRows;
      int iMaxPads;
      int iMinTimeBin;
      int iMaxTimeBin;
      bool iEnableNoiseSim;
      bool iEnablePedestalSubtraction;
      bool iIsContinuousReadout;
      CalDet<float>* iNoiseObject;
      CalDet<float>* iPedestalObject;
    };
    
    static void processDigits(
        const std::vector<std::vector<Digit const*>>& digits, 
        const std::vector<std::vector<HwClusterFinder*>>& clusterFinder, 
              std::vector<HwCluster>& cluster, 
              CfConfig config);
//              int iCRU,
//              int maxRows,
//              int maxPads, 
//              unsigned minTimeBin,
//              unsigned maxTimeBin);
    
    void ProcessTimeBins(int iTimeBinMin, int iTimeBinMax);

    std::vector<std::vector<std::vector<HwClusterFinder*>>> mClusterFinder;
    std::vector<std::vector<std::vector<Digit const*>>> mDigitContainer;

    std::vector<std::vector<HwCluster>> mClusterStorage;
    
    Processing    mProcessingType; 

    int     mGlobalTime;
    int     mCRUMin;
    int     mCRUMax;
    float   mMinQDiff;
    bool    mAssignChargeUnique;
    bool    mEnableNoiseSim;
    bool    mEnablePedestalSubtraction;
    bool    mIsContinuousReadout; ///< Switch for continuous readout
    int     mPadsPerCF;
    int     mTimebinsPerCF;
    int     mCfPerRow;
    int     mLastTimebin;

    std::vector<o2::TPC::HwCluster>* mClusterArray;    ///< Internal cluster storage
    
    CalDet<float>* mNoiseObject;
    CalDet<float>* mPedestalObject;
  };
}
}


#endif 
