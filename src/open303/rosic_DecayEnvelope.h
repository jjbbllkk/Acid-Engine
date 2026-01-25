#ifndef rosic_DecayEnvelope_h
#define rosic_DecayEnvelope_h

// rosic-indcludes:
#include "rosic_RealFunctions.h"

namespace rosic
{

  /**
  MODIFIED: Now includes an Attack phase for authentic 303 capacitor charging simulation.
  */

  class DecayEnvelope
  {

  public:

    //---------------------------------------------------------------------------------------------
    // construction/destruction:

    /** Constructor. */
    DecayEnvelope();  

    /** Destructor. */
    ~DecayEnvelope(); 

    //---------------------------------------------------------------------------------------------
    // parameter settings:

    /** Sets the sample-rate. */
    void setSampleRate(double newSampleRate);  

    /** Sets the attack time in milliseconds. */
    void setAttack(double newAttackTime);

    /** Sets the decay time constant in milliseconds. */
    void setDecayTimeConstant(double newTimeConstant);

    /** Switches into a mode where the normalization is not made with respect to the peak amplitude 
    but to the sum of the impulse response. */
    void setNormalizeSum(bool shouldNormalizeSum);

    //---------------------------------------------------------------------------------------------
    // inquiry:

    /** Returns the length of the decay phase (in milliseconds). */
    double getDecayTimeConstant() const { return tau; }

    /** True, if output is below some threshold. */
    bool endIsReached(double threshold);  

    //---------------------------------------------------------------------------------------------
    // audio processing:

    /** Calculates one output sample at a time. */
    INLINE double getSample();    

    //---------------------------------------------------------------------------------------------
    // others:

    /** Triggers the envelope. */
    void trigger();

  protected:

    /** Calculates the coefficient for multiplicative accumulation. */
    void calculateCoefficient();

    double c;             // coefficient for decay
    double attackCoeff;   // coefficient for attack (charging)
    double y;             // current output
    double yInit;         // target peak (usually 1.0)
    
    double attackTime;    // attack time in ms
    double tau;           // decay time in ms
    
    double time;          // timer for the attack phase
    double timeIncrement; // ms per sample

    double fs;            // sample-rate
    bool   normalizeSum; 

  };

  //-----------------------------------------------------------------------------------------------
  // inlined functions:

  INLINE double DecayEnvelope::getSample()
  {
    if(time < attackTime) {
        // ATTACK PHASE: Charge the capacitor (asymptotic approach to 1.0)
        // y = y + coeff * (target - y)
        y += attackCoeff * (yInit - y);
        time += timeIncrement;
    } else {
        // DECAY PHASE: Discharge the capacitor (exponential decay)
        y *= c;
    }
    return y;
  }

} // end namespace rosic

#endif // rosic_DecayEnvelope_h