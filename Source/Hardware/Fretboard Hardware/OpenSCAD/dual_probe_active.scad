//
// Dual-Probe Assembly with Semi-Circular Servo Horn (Fixed Syntax)
// The horn covers both probes when centered and frees the left probe when rotated right.
//
// Author: ChatGPT
//

// ===== PARAMETERS =====
probe_dia = 2.0;
spring_od = 4.5;
probe_spacing = 20.0;
guide_height = 25.0;
base_thickness = 10.0;
crossbar_thickness = 4.0;

horn_outer_R = probe_spacing / 2 + spring_od / 2 + 2.0;
horn_radial_thk = 4.0;
horn_z_thk = 4.0;
horn_z_offset = 2.0;
pivot_hole_dia = 4.0;

pocket_arc_angle = 50;    // how wide the cutout is
pocket_radial_extra = 6;  // how deep inward it goes
pocket_depth = 3;         // how far up into horn it cuts

$fn = 120;

// ===== MODULES =====

module base_plate() {
    translate([-10, -10, 0])
        cube([20, 20, base_thickness]);
}

module probe_guide(xoff) {
    translate([xoff, 0, base_thickness])
        cylinder(h = guide_height, d = probe_dia + 0.4);
}

module crossbar() {
    translate([-probe_spacing/2 - 3, -3, base_thickness + guide_height])
        cube([probe_spacing + 6, 6, crossbar_thickness]);
}

// Semi-circle horn with underside pocket
module semicircle_horn_with_pocket() {
    outer_R = horn_outer_R;
    inner_R = outer_R - horn_radial_thk;
    horn_z = base_thickness + guide_height + crossbar_thickness + horn_z_offset;

    // Base semi-circle (front half only)
    difference() {
        // Full ring
        translate([0,0,horn_z])
            difference() {
                cylinder(h = horn_z_thk, r = outer_R);
                translate([0,0,-0.1]) cylinder(h = horn_z_thk + 0.2, r = inner_R);
            }

        // Cut away back half so only front semi-circle remains
        translate([-outer_R, 0, horn_z - 1])
            cube([2*outer_R, outer_R, horn_z_thk + 2]);

        // Create pocket on right side (to free left probe when horn rotates right)
        rotate([0,0,-pocket_arc_angle/2])
            translate([0,0,horn_z - pocket_depth])
                intersection() {
                    // annular section to cut into horn
                    difference() {
                        cylinder(h = pocket_depth + 1, r = outer_R + 1);
                        translate([0,0,-0.1]) cylinder(h = pocket_depth + 1.2, r = inner_R - pocket_radial_extra);
                    }
                    // limit pocket angular extent
                    rotate_extrude(angle = pocket_arc_angle)
                        translate([inner_R - pocket_radial_extra,0,0])
                            square([outer_R - inner_R + pocket_radial_extra + 1, 1]);
                }

        // Pivot hole
        translate([0,0,horn_z - 1])
            cylinder(h = horn_z_thk + 2, d = pivot_hole_dia);
    }
}

// ===== ASSEMBLY =====
union() {
    base_plate();
    probe_guide(-probe_spacing/2);
    probe_guide(probe_spacing/2);
    crossbar();
    semicircle_horn_with_pocket();
}
