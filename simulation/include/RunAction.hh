#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include <map>
#include <string>
#include <fstream>

class G4Run;

class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);
    
    void AddSecondaryParticle(const G4String& name) { fSecondaryParticles[name]++; }
    
    // Record particle data to Excel
    void RecordParticleToExcel(const G4String& name, 
                              const G4ThreeVector& position);
                              
    // Count particle for summary
    void CountParticle(const G4String& name) { fParticleCounts[name]++; }

  private:
    std::map<G4String, int> fSecondaryParticles;
    std::map<G4String, int> fParticleCounts;  // For tracking all particles
    std::ofstream fOutputFile;
};

#endif