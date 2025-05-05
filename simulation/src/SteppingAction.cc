#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "RunAction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

SteppingAction::SteppingAction(EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fScoringVolume(nullptr),
  fDetector1Volume(nullptr),
  fDetector2Volume(nullptr)
{}

SteppingAction::~SteppingAction()
{
  // Print out the particle count at the end, filtering for muons and pions only
  G4cout << "=== Muons and Pions Generated ===" << G4endl;
  for (auto const& pair : fParticleCounter) {
    // Only include muons and pions in the summary
    if (pair.first == "mu+" || pair.first == "mu-" || 
        pair.first == "pi+" || pair.first == "pi-" || pair.first == "pi0") {
      G4cout << pair.first << ": " << pair.second << G4endl;
    }
  }
  G4cout << "===================================" << G4endl;
  
  // Add a section for particles detected at detector 1
  G4cout << "\n=== Muons and Pions Detected at Detector 1 ===" << G4endl;
  for (auto const& pair : fDetector1Particles) {
    G4cout << pair.first << ": " << pair.second << G4endl;
  }
  G4cout << "==========================================" << G4endl;
  
  // Add a section for particles detected at detector 2 (10m counter)
  G4cout << "\n=== Muons and Pions Detected at 10m (Detector 2) ===" << G4endl;
  for (auto const& pair : fDetector2Particles) {
    G4cout << pair.first << ": " << pair.second << G4endl;
  }
  G4cout << "================================================" << G4endl;
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  

  if (!fScoringVolume || !fDetector1Volume || !fDetector2Volume) { 
    const DetectorConstruction* detectorConstruction
      = static_cast<const DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume = detectorConstruction->GetScoringVolume();
    fDetector1Volume = detectorConstruction->GetDetector1Volume();
    fDetector2Volume = detectorConstruction->GetDetector2Volume();
    
    G4cout << "Detector 1 position: " << detectorConstruction->GetDetector1Position()/cm << " cm" << G4endl;
    G4cout << "Detector 2 position: " << detectorConstruction->GetDetector2Position()/cm << " cm" << G4endl;

  }


    // Now that we have detectorConstruction, print the positions
    
  // Get the RunAction - using const_cast to handle the constness issue
  const G4UserRunAction* constRunAction = G4RunManager::GetRunManager()->GetUserRunAction();
  RunAction* runAction = const_cast<RunAction*>(dynamic_cast<const RunAction*>(constRunAction));

  // Get current track
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* particle = track->GetDefinition();
  G4String particleName = particle->GetParticleName();
  G4double energy = track->GetKineticEnergy();
  
  // Check for pion decay specifically
  G4String processName = "Unknown";
  const G4VProcess* process = step->GetPostStepPoint()->GetProcessDefinedStep();
  if (process) processName = process->GetProcessName();
  
  // If this is a decay process
  if (processName == "Decay") {
    // If the current particle is a pion
    if (particleName == "pi+" || particleName == "pi-") {
      // Get secondaries created in this step
      const std::vector<const G4Track*>* secondaries = step->GetSecondaryInCurrentStep();
      
      if (secondaries && secondaries->size() > 0) {
        for (const G4Track* secTrack : *secondaries) {
          G4String secName = secTrack->GetDefinition()->GetParticleName();
          
          // If a muon is created from pion decay
          if (secName == "mu+" || secName == "mu-") {
            G4ThreeVector position = step->GetPostStepPoint()->GetPosition();
            G4double secEnergy = secTrack->GetKineticEnergy();
            
            G4cout << "\n!!! PION DECAY DETECTED !!!" << G4endl;
            G4cout << particleName << " → " << secName << G4endl;
            G4cout << "Position: " << position/mm << " mm" << G4endl;
            G4cout << "Parent Energy: " << energy/MeV << " MeV" << G4endl;
            G4cout << "Muon Energy: " << secEnergy/MeV << " MeV" << G4endl;
          }
        }
      }
    }
  }
  
  
  // Get current volume
  G4LogicalVolume* volume 
    = step->GetPreStepPoint()->GetTouchableHandle()
      ->GetVolume()->GetLogicalVolume();
  
  // Check for muons and charged pions in detector 1
  if (step->IsFirstStepInVolume() && volume == fDetector1Volume) {
    if (particleName == "mu+" || particleName == "mu-") {
      // Count muons
      fDetector1Particles[particleName]++;
      runAction->RecordParticleToExcel(particleName, energy);
      // Add to event counts
      if (fEventAction) {
        fEventAction->AddMuonAtDetector1();
      }
      
      G4cout << "\n!!! MUON DETECTED IN DETECTOR 1 !!!" << G4endl;
      G4cout << "Type: " << particleName << G4endl;
      G4cout << "Energy: " << track->GetKineticEnergy()/MeV << " MeV" << G4endl;
    }
    // Only count charged pions (pi+, pi-)
    else if (particleName == "pi+" || particleName == "pi-") {
      // Count charged pions
      fDetector1Particles[particleName]++;
      runAction->RecordParticleToExcel(particleName, energy);
      // Add to event counts
      if (fEventAction) {
        fEventAction->AddPionAtDetector1();
      }
      
      G4cout << "\n!!! PION DETECTED IN DETECTOR 1 !!!" << G4endl;
      G4cout << "Type: " << particleName << G4endl;
      G4cout << "Energy: " << track->GetKineticEnergy()/MeV << " MeV" << G4endl;
    }
  }

  // Check for muons and charged pions in detector 2 (10m counter)
  if (step->IsFirstStepInVolume() && volume == fDetector2Volume) {
    if (particleName == "mu+" || particleName == "mu-") {
      // Count muons at Detector 2
      fDetector2Particles[particleName]++;
      particleName="2"+particleName;
      runAction->RecordParticleToExcel(particleName, energy);
      // Add to event counts
      if (fEventAction) {
        fEventAction->AddMuonAtDetector2();
      }
      
      G4cout << "\n!!! MUON DETECTED AT 10m (DETECTOR 2) !!!" << G4endl;
      G4cout << "Type: " << particleName << G4endl;
      G4cout << "Energy: " << track->GetKineticEnergy()/MeV << " MeV" << G4endl;
    }
    // Only count charged pions (pi+, pi-)
    else if (particleName == "pi+" || particleName == "pi-") {
      // Count charged pions at Detector 2
      fDetector2Particles[particleName]++;
      particleName="2"+particleName;
      runAction->RecordParticleToExcel(particleName, energy);
      // Add to event counts
      if (fEventAction) {
        fEventAction->AddPionAtDetector2();
      }
      
      G4cout << "\n!!! PION DETECTED AT 10m (DETECTOR 2) !!!" << G4endl;
      G4cout << "Type: " << particleName << G4endl;
      G4cout << "Energy: " << track->GetKineticEnergy()/MeV << " MeV" << G4endl;
      // In SteppingAction.cc - add this to track all steps
      G4cout << "Step at position: " << step->GetPostStepPoint()->GetPosition()/CLHEP::m 
       << " m, Particle: " << particleName << G4endl;
    }
  }


  // Check if we are in scoring volume for energy deposition
  if (volume == fScoringVolume) {
    // Collect energy deposited in this step
    G4double edepStep = step->GetTotalEnergyDeposit();
    fEventAction->AddEdep(edepStep);
  }
}