#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class ElectricFieldSetup;  // Rename as needed but keep using this for magnetic field

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction() override;

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;  // Add this declaration
    
    // Methods to get the scoring volumes
    G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }
    G4LogicalVolume* GetDetector1Volume() const { return fDetector1Volume; }
    G4LogicalVolume* GetDetector2Volume() const { return fDetector2Volume; }

    // In DetectorConstruction.hh, add these to the public section:
    G4ThreeVector GetDetector1Position() const { return fDetector1Position; }
    G4ThreeVector GetDetector2Position() const { return fDetector2Position; }
    
  private:
    G4LogicalVolume* fScoringVolume;
    G4LogicalVolume* fDetector1Volume;
    G4LogicalVolume* fDetector2Volume;
    
    ElectricFieldSetup* fElectricFieldSetup;  // We'll keep the same name for simplicity
    // In the private section of DetectorConstruction.hh:
    G4ThreeVector fDetector1Position;
    G4ThreeVector fDetector2Position;
    
};

#endif