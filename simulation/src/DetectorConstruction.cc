#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "ElectricFieldSetup.hh"

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction(),
  fScoringVolume(nullptr),
  fDetector1Volume(nullptr)
{
}

DetectorConstruction::~DetectorConstruction()
{
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  G4NistManager* nist = G4NistManager::Instance();
  
  // World material: Air
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  // Tungsten material
  G4Material* tungsten_mat = nist->FindOrBuildMaterial("G4_W");
  
  // Scintillator material for detectors (plastic scintillator)
  G4Material* scintillator_mat = nist->FindOrBuildMaterial("G4_Ar");

  // World volume parameters
  G4double world_size = 10000*cm;

  // Tungsten block parameters - 10×10×30 cm
  G4double tungsten_x = 5*cm;
  G4double tungsten_y = 5*cm; 
  G4double tungsten_z = 100*cm;
  
  // Detector parameters - circular discs with 30 cm diameter and 1 cm thickness
  G4double detector_radius = 75*cm;  // 30 cm diameter
  G4double detector_thickness = 0.1*cm;
  
  // Detector positions (distance from the end of the tungsten block)
  G4double detector1_position = tungsten_z/2 + 60*cm;  // 10 cm from tungsten

  // World volume - cylindrical
G4double world_radius = 0.5*world_size;  // Radius matching the box half-width
G4double world_length = 20*world_size;   // Length matching the box z-dimension

G4Tubs* solidWorld = 
  new G4Tubs("World",
             0,                // inner radius
             world_radius,     // outer radius
             0.5*world_length, // half-length in z
             0*deg,            // starting angle
             360*deg);         // segment angle
  
G4LogicalVolume* logicWorld = 
  new G4LogicalVolume(solidWorld, world_mat, "World");
  
G4VPhysicalVolume* physWorld = 
  new G4PVPlacement(nullptr,               // no rotation
                    G4ThreeVector(),       // at (0,0,0)
                    logicWorld,            // its logical volume
                    "World",               // its name
                    nullptr,               // its mother volume
                    false,                 // no boolean operation
                    0,                     // copy number
                    true);                 // checking overlaps

  // Tungsten block
  G4Box* solidTungsten = 
    new G4Box("Tungsten", 0.5*tungsten_x, 0.5*tungsten_y, 0.5*tungsten_z);
  
  G4LogicalVolume* logicTungsten = 
    new G4LogicalVolume(solidTungsten, tungsten_mat, "Tungsten");
  
  new G4PVPlacement(nullptr,                // no rotation
                    G4ThreeVector(0, 0, 500), // at (0,0,0)
                    logicTungsten,          // its logical volume
                    "Tungsten",             // its name
                    logicWorld,             // its mother volume
                    false,                  // no boolean operation
                    0,                      // copy number
                    true);                  // checking overlaps

  // Create circular detectors (discs)
  G4Tubs* solidDetector1 = 
    new G4Tubs("Detector1", 
              0*cm,                   // inner radius
              detector_radius,        // outer radius
              0.5*detector_thickness, // half-length in z
              0*deg,                  // start angle
              360*deg);              // spanning angle             // spanning angle
  
  // Detector 1 (2 cm from tungsten)
  G4LogicalVolume* logicDetector1 = 
    new G4LogicalVolume(solidDetector1, scintillator_mat, "Detector1");
  
  new G4PVPlacement(nullptr,                // no rotation
                    G4ThreeVector(0, 0, detector1_position), // position
                    logicDetector1,         // its logical volume
                    "Detector1",            // its name
                    logicWorld,             // its mother volume
                    false,                  // no boolean operation
                    0,                      // copy number
                    true);                  // checking overlaps
  
    G4Tubs* solidDetector2 = 
    new G4Tubs("Detector1", 
              0*cm,                   // inner radius
              detector_radius,        // outer radius
              0.5*detector_thickness, // half-length in z
              0*deg,                  // start angle
              360*deg);              // spanning angle             // spanning angle
  
  // Detector 1 (2 cm from tungsten)
  G4LogicalVolume* logicDetector2 = 
    new G4LogicalVolume(solidDetector1, scintillator_mat, "Detector2");
  
  new G4PVPlacement(nullptr,                // no rotation
                    G4ThreeVector(0, 0, detector1_position+900*cm), // position
                    logicDetector1,         // its logical volume
                    "Detector2",            // its name
                    logicWorld,             // its mother volume
                    false,                  // no boolean operation
                    0,                      // copy number
                    true);                  // checking overlaps
  
  
  // Visual attributes
  G4VisAttributes* tungsten_vis_att = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5)); // Grey
  logicTungsten->SetVisAttributes(tungsten_vis_att);
  
  G4VisAttributes* detector1_vis_att = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0)); // Blue
  detector1_vis_att->SetVisibility(true);
  logicDetector1->SetVisAttributes(detector1_vis_att);

  G4VisAttributes* detector2_vis_att = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0)); // Blue
  detector1_vis_att->SetVisibility(true);
  logicDetector1->SetVisAttributes(detector2_vis_att);
  
  // Make the world volume transparent
  G4VisAttributes* world_vis_att = new G4VisAttributes(G4Colour(1.5, 1.5, 1.5, 1.0)); // Transparent
  world_vis_att->SetVisibility(true);
  world_vis_att->SetForceWireframe(true);
  logicWorld->SetVisAttributes(world_vis_att);

  // In DetectorConstruction::Construct()
  // Set scoring volumes
  fScoringVolume = logicTungsten;
  fDetector1Volume = logicDetector1;
  fDetector2Volume = logicDetector2;

  return physWorld;
}



void DetectorConstruction::ConstructSDandField()
{
  // Create global magnetic field
  if (!fElectricFieldSetup) {
    fElectricFieldSetup = new ElectricFieldSetup();
    G4ThreeVector fieldValue = G4ThreeVector(0.0, 0.0, 1.0*tesla);
    fElectricFieldSetup->SetMagneticField(fieldValue);
    
    G4cout << "\n-----------------------------------------------------------" << G4endl;
    G4cout << " Global Magnetic Field Set to: 0, 0, 1 Tesla" << G4endl;
    G4cout << "-----------------------------------------------------------\n" << G4endl;
  }
}