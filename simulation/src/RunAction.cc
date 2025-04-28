#include "RunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4UnitsTable.hh"

RunAction::RunAction()
: G4UserRunAction()
{}

RunAction::~RunAction()
{
  if (fOutputFile.is_open()) {
    fOutputFile.close();
  }
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
  fSecondaryParticles.clear();
  fParticleCounts.clear();
  
  // Open Excel file for particle data
  G4String fileName = "particle_data_run" + std::to_string(run->GetRunID()) + ".csv";
  fOutputFile.open(fileName);
  
  // Write CSV header with more information
  if (fOutputFile.is_open()) {
    fOutputFile << "ParticleType\tPosX_mm\tPosY_mm\tPosZ_mm" << std::endl;
    G4cout << "Recording particle data to file: " << fileName << G4endl;
  } else {
    G4cerr << "ERROR: Could not open output file " << fileName << G4endl;
  }
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;
  
  // Print simple particle summary
  G4cout << "\n=== PARTICLE SUMMARY ===" << G4endl;
  for (const auto& pair : fParticleCounts) {
    G4cout << pair.first << ": " << pair.second << G4endl;
  }
  G4cout << "=========================" << G4endl;
  
  // Close Excel file
  if (fOutputFile.is_open()) {
    fOutputFile.close();
    G4cout << "Particle data saved to Excel file" << G4endl;
  }
}

void RunAction::RecordParticleToExcel(const G4String& name, 
                                     const G4ThreeVector& position)
{
  if (fOutputFile.is_open()) {
    G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    
    // Write to Excel with enhanced information
    fOutputFile
                << name << "\t"
                << position.x()/mm << "\t"
                << position.y()/mm << "\t"
                << position.z()/mm << "\t"<<std::endl;
  }
  
  // Count this particle type for the summary
  CountParticle(name);
}