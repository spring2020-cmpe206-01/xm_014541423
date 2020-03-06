/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011, 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Author: Mahima Agumbe Suresh <mahima.agumbesuresh@sjsu.edu>
 * 
 */
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/enum.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/mobility-model.h"
#include <cmath>

#include "lognormal-shadowing-loss-model.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LogNormalPropagationLossModel");

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED (LogNormalPropagationLossModel);

// TODO: Add attributes required for this model. 
// Use the LogDistancePropagationLossModel and RandomPropagationLossModel as reference
// Two sample attributes are provided below for reference
TypeId
LogNormalPropagationLossModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LogNormalPropagationLossModel")
    .SetParent<PropagationLossModel> ()
    .SetGroupName ("Propagation")
    .AddConstructor<LogNormalPropagationLossModel> ()
    .AddAttribute ("Exponent",
                   "The exponent of the Path Loss propagation model",
                   DoubleValue (3.0),
                   MakeDoubleAccessor (&LogNormalPropagationLossModel::m_exponent),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("ReferenceDistance",
                   "The distance at which the reference loss is calculated (m)",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&LogNormalPropagationLossModel::m_referenceDistance),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("randVariable", "The random variable used to pick a loss every time CalcRxPower is invoked.",
                   StringValue ("ns3::NormalRandomVariable[Mean=0|Variance=0]"),
                   MakePointerAccessor (&LogNormalPropagationLossModel::m_randVariable),
                   MakePointerChecker<RandomVariableStream> ()) 
    .AddAttribute ("ReferenceLoss",
                   "The reference loss at reference distance (dB). (Default is Friis at 1m with 5.15 GHz)",
                   DoubleValue (46.6777),
                   MakeDoubleAccessor (&LogNormalPropagationLossModel::m_referenceLoss),
                   MakeDoubleChecker<double> ())            
  ;
  return tid;

}

LogNormalPropagationLossModel::LogNormalPropagationLossModel ()
{
}

void
LogNormalPropagationLossModel::SetPathLossExponent (double n)
{
  m_exponent = n;
}
void
LogNormalPropagationLossModel::SetReference (double referenceDistance, double referenceLoss)
{
  m_referenceDistance = referenceDistance;
  m_referenceLoss = referenceLoss;
}
double
LogNormalPropagationLossModel::GetPathLossExponent (void) const
{
  return m_exponent;
}

double
LogNormalPropagationLossModel::DoCalcRxPower (double txPowerDbm,
                                                Ptr<MobilityModel> a,
                                                Ptr<MobilityModel> b) const
{
	// TODO: Write your implementation of the RX power with this model
  double rxc_random = -m_randVariable->GetValue ();
  // //NS_LOG_DEBUG ("attenuation coefficient="<<rxc<<"Db");
  // //return txPowerDbm + rxc;
  
  double distance = a->GetDistanceFrom (b);
  if (distance <= m_referenceDistance){
    return txPowerDbm - m_referenceLoss-rxc_random;
  }

  
  /**
   * For the LogDistancePropagationLossModel, the formula is:
   * rx = 10 * log (Pr0(tx)) - n * 10 * log (d/d0)
   *
   * Pr0: rx power at reference distance d0 (W)
   * d0: reference distance: 1.0 (m)
   * d: distance (m)
   * tx: tx power (dB)
   * rx: dB
   *
   * Which, in our case is:
   *
   * rx = rx0(tx) - 10 * n * log (d/d0) -rxc_random
   */
   
  double pathLossDb = 10 * m_exponent * std::log10 (distance / m_referenceDistance);
  double rxc = -m_referenceLoss - pathLossDb+rxc_random;
  NS_LOG_DEBUG ("distance="<<distance<<"m, reference-attenuation="<< -m_referenceLoss<<"dB, "<<
                "attenuation coefficient="<<rxc<<"db");
  return txPowerDbm + rxc;

  //return txPowerDbm;
}

int64_t
LogNormalPropagationLossModel::DoAssignStreams (int64_t stream)
{
  m_randVariable->SetStream (stream);
  return 1;
}


} // namespace ns3
