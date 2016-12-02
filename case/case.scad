// All units are in mm

// Circle approximation constant
$fn=80;

// Epsilon value
e = 0.01;

// Wall thickness
wall = 1.6;

// Self-mating clearance, on each side
clearance = .15;

// Box's entire dimensions
width = 140;
length = 150;
height = 50;

// Adapter's screw-hole measurements
adapter_width = 24.105;
adapter_length = 33.63;

// GCN port measurements
gcn_thickness = 1.80;   // Measured at 1.51mm
gcn_diameter = 18.0;    // Measured at 16.81mm

// Xbox One controller's USB port
usb_width = 5.55;       // Measured at 5.15mm

// M2.5 pan head, 5mm tall
// 2.5mm diameter, with .2mm clearance(.1mm radius)
module screw_hole() {
    difference() {
        cylinder(h=6, d=8);
        translate([0,0,1])
            cylinder(h=6, d=2.5+.1);
    }
}

module screw_point() {
    union() {
        cylinder(h=6, d=3);
        //cylinder(h=1.5, d=5);
    }
}

module zip_tie() {
    scale([4,4,4]) {
        union() {
            cube([1,1,2]);
            translate([3,0,0])
                cube([1,1,2]);
            translate([0,0,1])
                cube([4,1,1]);
        }
    }
}

//zip_tie();
//screw_hole();
//screw_point();

union() {
    // Bottom base
    difference() {
        // Main rect
        cube([width, length, wall]);

        // Screw points
        translate([width - wall - 3, 2*wall + gcn_thickness + 3, -e])
            screw_point();
        translate([width - wall - 3, length - wall - 3, -e])
            screw_point();
    }

    // Screw collumns
    difference() {
        translate([wall, 2*wall + gcn_thickness, 0])
            cube([6, 6, height - wall - clearance]);
        translate([wall + 3, 2*wall + gcn_thickness + 3, height - wall - 6])
            cylinder(h=6, d=2.5+.1);
    }
    difference() {
        translate([wall, length - wall - 6, 0])
            cube([6, 6, height - wall - clearance]);
        translate([wall + 3, length - wall - 3, height - wall - 6])
            cylinder(h=6, d=2.5+.1);
    }

    // Adapter board mount points
    translate([width/2, 30, wall - 1]) {
        translate([-adapter_width/2, 0, 0])
            screw_hole();
        translate([adapter_width/2, 0, 0])
            screw_hole();
        translate([-adapter_width/2, 0 + adapter_length, 0])
            screw_hole();
        translate([adapter_width/2, 0 + adapter_length, 0])
            screw_hole();
    }

    // Xbox controller zip tie points
    translate([width/2 - 45, 50 + adapter_length, wall])
        zip_tie();
    translate([width/2 - 30, 105 + adapter_length, wall])
        zip_tie();
    translate([width/2 + 45, 50 + adapter_length, wall])
        zip_tie();
    translate([width/2 + 30, 105 + adapter_length, wall])
        zip_tie();

    // Front wall
    difference() {
        // Full front
        cube([width - wall - clearance, wall*2 + gcn_thickness, height/2 - clearance]);

        // Subtract out inner hollow spots for GCN connector
        translate([width/3 - gcn_diameter/2, wall, height/2 - gcn_diameter/2])
            cube([gcn_diameter, gcn_thickness, height/2+e]);
        translate([2*width/3 - gcn_diameter/2, wall, height/2 - gcn_diameter/2])
            cube([gcn_diameter, gcn_thickness, height/2+e]);

        // Subtract out cylinders for GCN connector
        translate([width/3, -e, height/2])
            rotate([270,0,0])
                cylinder(h=10, d=gcn_diameter);
        translate([2*width/3, -e, height/2])
            rotate([270,0,0])
                cylinder(h=10, d=gcn_diameter);

    }

    // Side wall
    cube([wall, length, height - wall - clearance]);

    // Back wall
    translate([0, length - wall, 0]) {
        difference() {
            // Full back
            cube([width - wall, wall, height - usb_width - wall - clearance]);

            // 50% cutaway
            translate([width/2 - clearance, -e, usb_width + wall - clearance])
                cube([width, wall + 2*e, height]);

            // Left side top USB slot
            translate([width/3 - usb_width/2, -e, height - clearance - 3/2*usb_width - wall])
                cube([usb_width, wall + 2*e, usb_width]);

            // Right-bottom USB slot
            translate([2*width/3 - usb_width/2, -e, wall - clearance + usb_width/2])
                cube([usb_width, wall + 2*e, usb_width]);
        }
    }
}
