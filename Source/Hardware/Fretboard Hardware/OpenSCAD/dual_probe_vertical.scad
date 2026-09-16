//
// Dual-Probe Vertical Version with SG90 Servo Mount
// Horn swings left/right between probes to block or free them.
//
// Author: ChatGPT
//

// ===== PARAMETERS =====
probe_dia = 2.0;          // Probe diameter
probe_spacing = 20.0;     // Distance between probe centers
guide_height = 25.0;      // Vertical travel guide
base_thickness = 5.0;     // Base plate
spring_clearance = 5.0;   // Gap under guide for springs
servo_w = 12.2;           // SG90 width
servo_h = 23.0;           // SG90 height
servo_d = 24.0;           // SG90 depth
horn_radius = probe_spacing/2 + 3.0;  // Horn half-circle radius
horn_thickness = 2.0;     // Horn plate thickness
horn_height = 20.0;       // Height of the horn
shaft_dia = 4.8;          // Servo shaft clearance
$fn = 80;

// ===== MODULES =====

// Base platform
module base_plate(){
    translate([-30,-servo_d/2,0])
        cube([60, servo_d, base_thickness]);
}

// Vertical probe guide tubes
module probe_guide(xoff){
    translate([xoff,0,base_thickness])
        cylinder(h = guide_height, d = probe_dia + 0.4);
}

// Servo mount block for SG90
module servo_mount(){
    // Outer housing
    translate([-servo_w/2, -servo_d/2, base_thickness])
        cube([servo_w, servo_d, servo_h + 2]);
    // Servo cavity
    difference(){
        translate([-servo_w/2, -servo_d/2, base_thickness])
            cube([servo_w, servo_d, servo_h + 2]);
        translate([-servo_w/2+0.6, -servo_d/2+0.6, base_thickness+0.6])
            cube([servo_w-1.2, servo_d-1.2, servo_h + 1]);
    }
}

// Vertical horn (semi-circular gate)
module vertical_horn(){
    difference(){
        // Main half-disc
        rotate([0,90,0])
            translate([0,0,-horn_thickness/2])
                linear_extrude(height = horn_thickness)
                    difference(){
                        circle(horn_radius);
                        circle(horn_radius - 2.0);
                        // Cut back half to form semicircle
                        translate([-horn_radius,-horn_radius])
                            square([2*horn_radius, horn_radius]);
                    }

        // Shaft hole
        rotate([0,90,0])
            translate([0,0,-horn_thickness])
                cylinder(h=2*horn_thickness, d=shaft_dia);
    }
}

// ===== ASSEMBLY =====
union(){
    base_plate();

    // Servo mount
    servo_mount();

    // Probe guides (left/right)
    probe_guide(-probe_spacing/2);
    probe_guide(probe_spacing/2);

    // Horn positioned at servo shaft height
    translate([0,0,base_thickness + servo_h/2])
        vertical_horn();
}
