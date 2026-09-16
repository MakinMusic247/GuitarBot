//
// Dual-Probe Assembly with Semi-Circular Servo Horn
// Ready for STL export (Tinkercad compatible)
//

// ==================== PARAMETERS ====================

// Probe & spring
probe_dia = 2.0;           // probe shaft diameter
spring_od = 4.5;           // spring outer diameter
probe_spacing = 20.0;      // spacing between probes

// Structure
crossbar_thickness = 4.0;  // thickness of crossbar
guide_height = 25.0;       // height of guide tubes
base_thickness = 10.0;     // base plate thickness
servo_width = 23.0;        // servo mount width
servo_height = 12.0;       // servo mount height

// Servo horn
horn_outer_R = probe_spacing/2 + spring_od/2 + 2; // outer radius of horn
horn_thickness_radial = 4;   // radial width (ring thickness)
horn_thickness_z = 4;        // vertical thickness
horn_z_offset = 2;           // offset above crossbar
pivot_hole_dia = 4;          // hole for servo screw or spline hub

// ==================== MODULES ====================

// Base plate
module base_plate() {
    cube([servo_width, servo_height, base_thickness], center=true);
}

// Probe guide
module probe_guide(xoff) {
    translate([xoff, 0, base_thickness])
        cylinder(h=guide_height, d=probe_dia+0.4, $fn=60);
}

// Crossbar
module crossbar() {
    crossbar_len = probe_spacing + 2*spring_od;
    translate([0, 0, base_thickness+guide_height+2])
        cube([crossbar_len, 6, crossbar_thickness], center=true);
}

// Semi-circular servo horn
module semicircle_horn() {
    outer_R = horn_outer_R;
    inner_R = outer_R - horn_thickness_radial;
    horn_z = base_thickness + guide_height + crossbar_thickness + horn_z_offset;

    // Main semicircular plate
    difference() {
        // full ring
        translate([0,0,horn_z])
            difference() {
                cylinder(h = horn_thickness_z, r = outer_R, $fn=160);
                translate([0,0,-1])
                    cylinder(h = horn_thickness_z+2, r = inner_R, $fn=120);
            }
        // Cut to semi-circle (keep front half only)
        translate([-outer_R*1.5, -outer_R*1.5, horn_z-1])
            cube([outer_R*3, outer_R, horn_thickness_z+2], center=false);
    }

    // Pivot hole
    translate([0, 0, horn_z])
        cylinder(h = horn_thickness_z+2, d = pivot_hole_dia, $fn=36);

    // Contact lip (downward)
    translate([0,0,horn_z - 0.6]) {
        rotate_extrude($fn=120)
            translate([inner_R+0.2, 0, 0])
                square([0.8, 0.8], center=true);
    }
}

// ==================== ASSEMBLY ====================

union() {
    base_plate();
    probe_guide(-probe_spacing/2);
    probe_guide(probe_spacing/2);
    crossbar();
    semicircle_horn();
}
