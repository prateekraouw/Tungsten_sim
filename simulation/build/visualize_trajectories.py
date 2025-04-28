#!/usr/bin/env python3
import sys
import os
import subprocess

def generate_visualization_macro(trajectory_file, output_macro):
    """Generate a G4 macro file to visualize trajectories"""
    
    # Read trajectory data
    trajectories = []
    current_traj = None
    points = []
    
    with open(trajectory_file, 'r') as f:
        for line in f:
            if line.startswith('#'):
                continue
                
            parts = line.strip().split()
            
            if len(parts) == 6:  # Header line
                # If we were processing a trajectory, save it
                if current_traj is not None:
                    current_traj['points'] = points
                    trajectories.append(current_traj)
                
                # Start a new trajectory
                current_traj = {
                    'event_id': int(parts[0]),
                    'track_id': int(parts[1]),
                    'parent_id': int(parts[2]),
                    'particle_type': parts[3],
                    'process': parts[4],
                    'num_points': int(parts[5])
                }
                points = []
            
            elif len(parts) == 4:  # Point line
                points.append({
                    'x': float(parts[0]),
                    'y': float(parts[1]),
                    'z': float(parts[2]),
                    'energy': float(parts[3])
                })
    
    # Add the last trajectory if there is one
    if current_traj is not None:
        current_traj['points'] = points
        trajectories.append(current_traj)
    
    # Generate macro file
    with open(output_macro, 'w') as f:
        # Initialize kernel and visualization
        f.write("# Initialize the kernel\n")
        f.write("/run/initialize\n\n")
        
        # Open visualization
        f.write("# Set up visualization\n")
        f.write("/vis/open OGL\n")
        f.write("/vis/drawVolume\n")
        f.write("/vis/viewer/set/style surface\n")
        f.write("/vis/viewer/set/viewpointThetaPhi 60 30\n")
        f.write("/vis/viewer/zoom 1.5\n")
        f.write("/vis/scene/add/axes 0 0 0 1 m\n\n")
        
        # Make world volume transparent
        f.write("# Make world volume transparent\n")
        f.write("/vis/geometry/set/visibility World 0 false\n\n")
        
        # Set up trajectory drawing
        f.write("# Set up trajectory drawing\n")
        f.write("/vis/scene/add/trajectories\n")
        f.write("/vis/modeling/trajectories/create/drawByParticleID\n")
        f.write("/vis/modeling/trajectories/drawByParticleID-0/set mu+ green\n")
        f.write("/vis/modeling/trajectories/drawByParticleID-0/set mu- green\n")
        f.write("/vis/modeling/trajectories/drawByParticleID-0/set pi+ red\n")
        f.write("/vis/modeling/trajectories/drawByParticleID-0/set pi- red\n")
        f.write("/vis/modeling/trajectories/drawByParticleID-0/set pi0 red\n\n")
        
        # Generate trajectories from file data
        f.write("# Generate trajectories from file data\n")
        
        for traj in trajectories:
            particle_type = traj['particle_type']
            
            # Skip non-muon/pion particles
            if not (particle_type.startswith('mu') or particle_type.startswith('pi')):
                continue
                
            points = traj['points']
            if len(points) < 2:
                continue
                
            # Start a polyline
            f.write("/vis/scene/add/userAction\n")
            
            # Set color based on particle type
            if particle_type.startswith('mu'):
                f.write("/vis/modeling/trajectories/drawByParticleID-0/set {} green\n".format(particle_type))
            else:
                f.write("/vis/modeling/trajectories/drawByParticleID-0/set {} red\n".format(particle_type))
                
            # Draw polyline for this trajectory
            f.write("/vis/scene/add/polyline\n")
            
            for i, point in enumerate(points):
                if i == 0:
                    f.write("/vis/scene/add/polylinePoint {} {} {}\n".format(
                        point['x'], point['y'], point['z']))
                else:
                    f.write("/vis/scene/add/polylinePoint {} {} {}\n".format(
                        point['x'], point['y'], point['z']))
            
            f.write("\n")
        
        # Update viewer
        f.write("# Update viewer\n")
        f.write("/vis/viewer/set/autoRefresh true\n")
        f.write("/vis/viewer/refresh\n")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: {} <trajectory_file>".format(sys.argv[0]))
        sys.exit(1)
        
    trajectory_file = sys.argv[1]
    if not os.path.exists(trajectory_file):
        print("Error: File '{}' does not exist".format(trajectory_file))
        sys.exit(1)
        
    output_macro = "visualize_trajectories.mac"
    generate_visualization_macro(trajectory_file, output_macro)
    
    print("Visualization macro generated: {}".format(output_macro))
    print("To use this with Geant4, run:")
    print("  ./tungsten_sim {}".format(output_macro))