#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

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

  private:
    G4LogicalVolume* fScoringVolume;
    G4LogicalVolume* fDetector1Volume;
    G4LogicalVolume* fDetector2Volume;
    ElectricFieldSetup* fElectricFieldSetup;  // We'll keep the same name for simplicity
};

#endif