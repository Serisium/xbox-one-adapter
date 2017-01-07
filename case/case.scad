// All units are in mm

// Circle approximation constant
$fn=80;

// Epsilon value
e = 0.01;

// Wall thickness
wall = 2.4;

// Self-mating clearance, on each side
clearance = .15;

// Box's entire dimensions
width = 135;
length = 130;
height = 40;

// Adapter's screw-hole measurements
adapter_width = 24.105;
adapter_length = 35.63;

// GCN port measurements
gcn_thickness = 1.80;   // Measured at 1.51mm
gcn_diameter = 18.0;    // Measured at 16.81mm

// Xbox One controller's USB port
usb_width = 5.55;       // Measured at 5.15mm

// M2.5 pan head, 5mm tall
// 2.5mm diameter, with .2mm clearance(.1mm radius)
module screw_container() {
    difference() {
        cylinder(h=7.5, d=8);
        translate([0,0,.5])
            screw_hole();
    }
}
module screw_hole() {
    union() {
        translate([0,0,-e])
            cylinder(h=100, d=3.6);
        translate([0,0,3])
            cylinder(d1=3.6, d2=4.04, h=3.51);
    }
}

module screw_point() {
    union() {
        cylinder(h=6, d=3);
        //cylinder(h=1.5, d=5);
    }
}

module zip_tie() {
    translate([0, 0, 2]) {
        union() {
            linear_extrude(height=4, center=true, scale=.5)
                square(4, center=true);
            translate([10,0,0])
                linear_extrude(height=4, center=true, scale=.5)
                    square(4, center=true);
            translate([5,0,1])
                cube([10,2,2], center=true);
        }
    }
}

//zip_tie();
//screw_container();
//screw_point();

union() {
    // Bottom base
    difference() {
        // Main rect
        cube([width, length, wall]);

        // Screw points
        translate([width - wall - 4, wall + gcn_thickness + 4, -e])
            screw_point();
        translate([width - wall - 4, length - wall - 4, -e])
            screw_point();
    }

    // Screw collumns
    difference() {
        translate([wall, wall + gcn_thickness, 0])
            cube([8, 8, height - wall - clearance]);
        translate([wall + 4, wall + gcn_thickness + 4, height - wall - 7.5])
            scale([.9, .9, 1])
                screw_hole();
    }
    difference() {
        translate([wall, length - wall - 8, 0])
            cube([8, 8, height - wall - clearance]);
        translate([wall + 4, length - wall - 4, height - wall - 7.5])
            scale([.9, .9, 1])
                screw_hole();
    }

    // Adapter board mount points
    translate([width/2, 15, wall - 1]) {
        translate([-adapter_width/2, 0, 0])
            screw_container();
        translate([adapter_width/2, 0, 0])
            screw_container();
        translate([-adapter_width/2, 0 + adapter_length, 0])
            screw_container();
        translate([adapter_width/2, 0 + adapter_length, 0])
            screw_container();
    }

    // Xbox controller zip tie points
    translate([width/2, 64, wall]) {
        translate([-45, 0, 0])
            zip_tie();
        translate([-30, 55, 0])
            zip_tie();
        translate([45, 0, 0])
            zip_tie();
        translate([30, 55, 0])
            zip_tie();
    }

    // Front wall
    difference() {
        // Full front
        cube([width - wall - 2*clearance, wall + gcn_thickness, height/2 - clearance]);

        // Subtract out inner hollow spots for GCN connector
        translate([width/4, wall, height/2])
            cube([gcn_diameter, gcn_thickness, height/2+e], center=true);
        translate([3*width/4, wall, height/2])
            cube([gcn_diameter, gcn_thickness, height/2+e], center=true);

        // Subtract out cylinders for GCN connector
        translate([width/4, -e, height/2])
            rotate([270,0,0])
                cylinder(h=10, d=gcn_diameter);
        translate([3*width/4, -e, height/2])
            rotate([270,0,0])
                cylinder(h=10, d=gcn_diameter);

    }

    // Side wall
    cube([wall, length, height - wall - clearance]);

    // Back wall
    translate([0, length - wall, 0]) {
        difference() {
            // Full back
            cube([width - wall, wall, height - usb_width - wall - 2*clearance]);

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

    // Side wall ribs
    for(i = [0:6]) {
        translate([wall, 20 + i*15, wall]) {
            linear_extrude(height = height - 2*wall - clearance, scale=[e, 1])
                square([4,2]);
        }
    }

    // Screw collumn ribs
    translate([wall+8, wall+gcn_thickness+3, wall]) {
        linear_extrude(height = height - 2*wall - clearance, scale=[e, 1])
            square([4,2]);
    }
    translate([wall+3, wall+gcn_thickness+8, wall]) {
        linear_extrude(height = height - 2*wall - clearance, scale=[1, e])
            square([2,4]);
    }
    translate([wall+8, length - wall - 5, wall]) {
        linear_extrude(height = height - 2*wall - clearance, scale=[e, 1])
            square([4,2]);
    }
    translate([wall+5, length - wall - 8, wall]) {
        rotate([0, 0, 180])
            linear_extrude(height = height - 2*wall - clearance, scale=[1, e])
                square([2,4]);
    }

    // Back wall ribs
    for(i = [0:3]) {
        translate([10 + i*15, length-wall, wall]) {
            rotate([0, 0, 180]) {
                linear_extrude(height = height - usb_width - 2*wall - clearance, scale=[1, e]) {
                    square([4,2]);
                }
            }
        }
    }
}
