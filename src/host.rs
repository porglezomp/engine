#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Axis {
    MoveUD,
    MoveLR,
    LookUD,
    LookLR,
}

pub struct Host {
    axes: [f32; 4],
    pub pos: [f32; 3],
    pub clear_color: [f32; 4],
}

impl Host {
    pub fn new() -> Self {
        Host {
            axes: [0.0; 4],
            pos: [0.0, 0.0, -1.0],
            clear_color: [0.0; 4],
        }
    }

    pub fn get_axis(&self, axis: Axis) -> f32 {
        match axis {
            Axis::LookLR => self.axes[0],
            Axis::LookUD => self.axes[1],
            Axis::MoveLR => self.axes[2],
            Axis::MoveUD => self.axes[3],
        }
    }

    pub fn set_axis(&mut self, axis: Axis, val: f32) {
        match axis {
            Axis::LookLR => self.axes[0] = val,
            Axis::LookUD => self.axes[1] = val,
            Axis::MoveLR => self.axes[2] = val,
            Axis::MoveUD => self.axes[3] = val,
        }
    }
}
