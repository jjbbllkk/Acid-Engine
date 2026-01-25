#include "rosic_DecayEnvelope.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

DecayEnvelope::DecayEnvelope()
{  
  c             = 1.0;    
  attackCoeff   = 1.0;
  y             = 0.0;
  yInit         = 1.0;
  
  tau           = 200.0;
  attackTime    = 0.0; // Default to instant
  
  time          = 0.0;
  fs            = 44100.0;
  timeIncrement = 1000.0 / fs; // ms per sample

  normalizeSum  = false;
  calculateCoefficient();
}

DecayEnvelope::~DecayEnvelope()
{

}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void DecayEnvelope::setSampleRate(double newSampleRate)
{
  if( newSampleRate > 0.0 )
  {
    fs = newSampleRate;
    timeIncrement = 1000.0 / fs;
    calculateCoefficient();
  }
}

void DecayEnvelope::setAttack(double newAttackTime)
{
    if(newAttackTime >= 0.0) {
        attackTime = newAttackTime;
        calculateCoefficient();
    }
}

void DecayEnvelope::setDecayTimeConstant(double newTimeConstant)
{
  if( newTimeConstant > 0.001 ) // at least 0.001 ms decay
  {
    tau = newTimeConstant;
    calculateCoefficient();
  }
}

void DecayEnvelope::setNormalizeSum(bool shouldNormalizeSum)
{
  normalizeSum = shouldNormalizeSum;
  calculateCoefficient();
}

//-------------------------------------------------------------------------------------------------
// others:

void DecayEnvelope::trigger()
{
  // Reset the timer to start the Attack Phase.
  // CRITICAL: Do NOT reset 'y' to 0.0 here. 
  // Real 303 capacitors charge from their *current* voltage.
  // This creates the authentic "slide" behavior if re-triggered quickly.
  time = 0.0;
}

bool DecayEnvelope::endIsReached(double threshold)
{
  if( y < threshold && time > attackTime )
    return true;
  else
    return false;
}

//-------------------------------------------------------------------------------------------------
// internal functions:

void DecayEnvelope::calculateCoefficient()
{
  // 1. Calculate Decay Coefficient (Multiplicative)
  c = exp( -1.0 / (0.001*tau*fs) );
  
  // 2. Calculate Attack Coefficient (Additive Asymptotic)
  // Logic: 1 - exp(-1 / (time_in_seconds * sample_rate))
  if (attackTime > 0.0) {
      attackCoeff = 1.0 - exp( -1.0 / (0.001 * attackTime * fs) );
  } else {
      attackCoeff = 1.0; // Instant jump
  }

  // 3. Normalize
  if( normalizeSum == true )
    yInit = (1.0-c)/c;
  else  
    yInit = 1.0/c; // Slightly > 1.0 to compensate for first multiply, keeps peak at 1.0
}