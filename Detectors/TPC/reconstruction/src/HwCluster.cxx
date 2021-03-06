// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file HwCluster.cxx
/// \brief Class to have some more info about the HwClusterer clusters

#include "TPCReconstruction/HwCluster.h"
#include "FairLogger.h"

#include <cmath>

ClassImp(o2::TPC::HwCluster)

using namespace o2::TPC;

//________________________________________________________________________
HwCluster::HwCluster()
  : HwCluster(5,5)
{}

//________________________________________________________________________
HwCluster::HwCluster(short sizeP, short sizeT)
  : Cluster()
  , mPad(-1)
  , mTime(-1)
  , mSizeP(sizeP)
  , mSizeT(sizeT)
  , mSize(0)
  , mClusterData(mSizeT, std::vector<float> (mSizeP, 0))
{
}

//________________________________________________________________________
HwCluster::HwCluster(short cru, short row, short sizeP, short sizeT, 
    float** clusterData, short maxPad, short maxTime)
  : Cluster(cru,row,-1,-1,-1,-1,-1,-1)
  , mPad(maxPad)
  , mTime(maxTime)
  , mSizeP(sizeP)
  , mSizeT(sizeT)
  , mSize(0)
  , mClusterData(mSizeT, std::vector<float> (mSizeP))
{
  short t,p;
  for (t=0; t<mSizeT; ++t){
    for (p=0; p<mSizeP; ++p){
      mClusterData[t][p] = clusterData[t][p];
    }
  }

  calculateClusterProperties();
}

//________________________________________________________________________
HwCluster::HwCluster(const HwCluster& other)
  = default;

//________________________________________________________________________
void HwCluster::setClusterData(short cru, short row, short sizeP, short sizeT, 
    float** clusterData, short maxPad, short maxTime)
{
  if (sizeP != mSizeP || sizeT != mSizeT) {
    LOG(ERROR) << "Given cluster size does not match. Abort..." << FairLogger::endl;
  }
  short t,p;
  for (t=0; t<mSizeT; ++t){
    for (p=0; p<mSizeP; ++p){
      mClusterData[t][p] = clusterData[t][p];
    }
  }
  mPad = maxPad;
  mTime = maxTime;

  calculateClusterProperties();
}

//________________________________________________________________________
void HwCluster::calculateClusterProperties()
{

  double qMax = mClusterData[2][2];   // central pad
  double qTot = 0;//qMax;
  double charge = 0;
  double meanP = 0;
  double meanT = 0;
  double sigmaP = 0;
  double sigmaT = 0;
  short minT = mSizeT;
  short maxT = 0;
  short minP = mSizeP;
  short maxP = 0;

  short deltaT, deltaP;
  short t, p;
  for (t = 0; t < mSizeT; ++t) {
    deltaT = t - mSizeT/2;
    for (p = 0; p < mSizeP; ++p) {
      deltaP = p - mSizeP/2;

      charge = mClusterData[t][p];

      qTot += charge;

      meanP += charge * deltaP;
      meanT += charge * deltaT;

      sigmaP += charge * deltaP*deltaP;
      sigmaT += charge * deltaT*deltaT;

      if (charge > 0) {
        minP = std::min(minP,p); maxP = std::max(maxP,p);
        minT = std::min(minT,t); maxT = std::max(maxT,t);
      }
      
    }
  }

  mSize = (maxP-minP+1)*10 + (maxT-minT+1);

  if (qTot > 0) {
    meanP  /= qTot;
    meanT  /= qTot;
    sigmaP /= qTot;
    sigmaT /= qTot;

    sigmaP = std::sqrt(sigmaP - (meanP*meanP));
    sigmaT = std::sqrt(sigmaT - (meanT*meanT));

    meanP += mPad;
    meanT += mTime;
  }

  setParameters(getCRU(),getRow(),(double)qTot,(double)qMax,(double)meanP,sigmaP,(double)meanT,sigmaT);
}

//________________________________________________________________________
std::ostream& HwCluster::print(std::ostream &output) const
{
  Cluster::print(output);
  output << " centered at (pad, time) = " << mPad << ", " << mTime
    << " covering " << (short)(mSize/10)  << " pads and " << mSize%10
    << " time bins";
  return output;
}

//________________________________________________________________________
std::ostream& HwCluster::PrintDetails(std::ostream &output) const
{
  Cluster::print(output);
  output << " centered at (pad, time) = " << mPad << ", " << mTime
    << " covering " << (short)(mSize/10)  << " pads and " << mSize%10
    << " time bins" << "\n";
  short t,p;
  for (t=0; t<mSizeT; ++t){
    for (p=0; p<mSizeP; ++p){
      output << "\t" << mClusterData[t][p];
    }
    output << "\n";
  }

  return output;
}

