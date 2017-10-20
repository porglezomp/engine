use cgmath::Matrix4;


#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Axis {
    MoveUD,
    MoveLR,
    LookUD,
    LookLR,
}

#[derive(Debug, Copy, Clone, Hash, PartialEq, Eq)]
pub struct ModelId(pub u32);
#[derive(Debug, Copy, Clone, Hash, PartialEq, Eq)]
pub struct ShaderId(pub u32);

#[derive(Debug, Clone)]
pub enum RenderCommand {
    ClearColor([f32; 4]),
    ClearDepth(f32),
    Model(ModelId, Matrix4<f32>),
}

#[derive(Debug)]
pub struct Host {
    pub render_queue: Vec<RenderCommand>,
    axes: [f32; 4],
}

impl Host {
    pub fn new() -> Self {
        Host {
            render_queue: Vec::with_capacity(1024),
            axes: [0.0; 4],
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

    pub fn send_render_command(&mut self, cmd: RenderCommand) {
        self.render_queue.push(cmd);
    }
}
