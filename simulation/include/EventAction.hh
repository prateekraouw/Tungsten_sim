#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class EventAction : public G4UserEventAction
{
public:
  EventAction();
  virtual ~EventAction();
  
  // These virtual methods must be declared here 
  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

  // Method to add energy deposit
  void AddEdep(G4double edep) { fEdep += edep; }
  G4double GetEdep() const { return fEdep; }
  
  // Methods for detector 1
  void AddMuonAtDetector1() { fMuonsAtDetector1++; }
  void AddPionAtDetector1() { fPionsAtDetector1++; }
  G4int GetMuonsAtDetector1() const { return fMuonsAtDetector1; }
  G4int GetPionsAtDetector1() const { return fPionsAtDetector1; }
  
  // Methods for detector 2 (10m counter)
  void AddMuonAtDetector2() { fMuonsAtDetector2++; }
  void AddPionAtDetector2() { fPionsAtDetector2++; }
  G4int GetMuonsAtDetector2() const { return fMuonsAtDetector2; }
  G4int GetPionsAtDetector2() const { return fPionsAtDetector2; }

private:
  G4double fEdep;  // Energy deposit
  
  // Counters for detector 1
  G4int fMuonsAtDetector1;
  G4int fPionsAtDetector1;
  
  // Counters for detector 2 (10m counter)
  G4int fMuonsAtDetector2;
  G4int fPionsAtDetector2;
};

#endif