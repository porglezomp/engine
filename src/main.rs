#[macro_use]
extern crate glium;
extern crate cgmath;
extern crate live_reload;

use live_reload::ShouldQuit;
use glium::Surface;
use glium::glutin::{Event, WindowEvent, VirtualKeyCode, ElementState, DeviceEvent};
use std::io::Read;
use cgmath::{conv, Matrix4, vec3};
// use cgmath::prelude::*;

mod host;

use host::{Host, Axis};

#[derive(Debug)]
enum Error {
    Io(usize, std::io::Error),
    ParseInt(usize, std::num::ParseIntError),
    ParseFloat(usize, std::num::ParseFloatError),
    BadCount(usize),
    MissingVertCount(usize),
    MissingIndexCount(usize),
}

#[derive(Copy, Clone, Debug)]
struct Vert {
    pos: [f32; 3],
    norm: [f32; 3],
}
implement_vertex!(Vert, pos, norm);

#[derive(Debug)]
struct ModelData {
    vertex: Vec<Vert>,
    index: Vec<u16>,
}

impl ModelData {
    pub fn load<R: std::io::BufRead>(input: &mut R) -> Result<ModelData, Error> {
        let mut buf = String::new();
        input.read_line(&mut buf).map_err(|e| Error::Io(1, e))?;
        let vert_count = {
            let parts: Vec<_> = buf.trim().split_whitespace().collect();
            if parts.len() != 3 || parts[0] != "vert" {
                return Err(Error::MissingVertCount(1));
            }
            let component_count: usize = parts[1].parse().map_err(|e| Error::ParseInt(1, e))?;
            if component_count != 6 {
                return Err(Error::BadCount(component_count));
            }
            parts[2].parse().map_err(|e| Error::ParseInt(1, e))?
        };

        let mut vertex = Vec::with_capacity(vert_count);
        for line in (0..vert_count).map(|x| x + 2) {
            buf.clear();
            input.read_line(&mut buf).map_err(|e| Error::Io(line, e))?;
            let parts = buf.trim()
                .split_whitespace()
                .map(|x| x.parse())
                .collect::<Result<Vec<_>, _>>()
                .map_err(|e| Error::ParseFloat(line, e))?;

            vertex.push(Vert {
                pos: [parts[0], parts[1], parts[2]],
                norm: [parts[3], parts[4], parts[5]],
            });
        }

        let idx_line = 2 + vert_count;
        buf.clear();
        input.read_line(&mut buf).map_err(
            |e| Error::Io(idx_line, e),
        )?;
        let index_count: usize = {
            let parts: Vec<_> = buf.trim().split_whitespace().collect();
            if parts.len() != 2 || parts[0] != "index" {
                return Err(Error::MissingIndexCount(idx_line));
            }
            parts[1].parse().map_err(|e| Error::ParseInt(idx_line, e))?
        };

        let mut index = Vec::with_capacity(index_count);
        for line in (0..index_count).map(|x| x + idx_line) {
            buf.clear();
            input.read_line(&mut buf).map_err(|e| Error::Io(line, e))?;
            for item in buf.trim().split_whitespace().map(|x| x.parse()) {
                index.push(item.map_err(|e| Error::ParseInt(line, e))?)
            }
        }

        Ok(ModelData { vertex, index })
    }
}

struct Model {
    vertex: glium::VertexBuffer<Vert>,
    index: glium::IndexBuffer<u16>,
}

pub fn main() {
    let mut events_loop = glium::glutin::EventsLoop::new();
    let window = glium::glutin::WindowBuilder::new()
        .with_dimensions(1280, 720)
        .with_title("Garden");
    let context = glium::glutin::ContextBuilder::new()
        .with_vsync(true)
        .with_depth_buffer(32);
    let display =
        glium::Display::new(window, context, &events_loop).expect("Should create display!");

    let host = Host::new();
    let mut app = live_reload::Reloadable::new("target/debug/libgame.dylib", host)
        .expect("Should load!");

    let file = std::fs::File::open("assets/models/well.model").unwrap();
    let mut buf_read = std::io::BufReader::new(file);
    let model_data = ModelData::load(&mut buf_read).expect("Should load");

    let model = Model {
        vertex: glium::vertex::VertexBuffer::new(&display, &model_data.vertex).unwrap(),
        index: glium::index::IndexBuffer::new(
            &display,
            glium::index::PrimitiveType::TrianglesList,
            &model_data.index,
        ).unwrap(),
    };

    let program = {
        let mut vert_file = std::fs::File::open("assets/shaders/basic.vert").unwrap();
        let mut vert_src = String::new();
        vert_file.read_to_string(&mut vert_src).unwrap();
        let mut frag_file = std::fs::File::open("assets/shaders/basic.frag").unwrap();
        let mut frag_src = String::new();
        frag_file.read_to_string(&mut frag_src).unwrap();
        glium::program::Program::from_source(&display, &vert_src, &frag_src, None).unwrap()
    };

    let proj = conv::array4x4(cgmath::perspective(
        cgmath::Deg(80.0f32),
        1280.0 / 720.0,
        0.1,
        500.0,
    ));

    let mut input = Input::new();

    let mut running = true;
    while running {
        app.reload().expect("Should safely reload!");

        running = input.handle_input(&mut events_loop, &mut app);

        if app.update() == ShouldQuit::Yes {
            running = false;
        }

        let mut frame = display.draw();
        let mut view_transform = <cgmath::Matrix4<f32> as cgmath::One>::one();
        for cmd in &app.host().render_queue {
            match *cmd {
                host::RenderCommand::ClearColor(col) => {
                    frame.clear_color(col[0], col[1], col[2], col[3])
                }
                host::RenderCommand::ClearDepth(d) => frame.clear_depth(d),
                host::RenderCommand::Camera(camera) => view_transform = camera,
                host::RenderCommand::Model(id, transform) => {
                    let _ = id;
                    let uniforms =
                        uniform!{
                            proj_from_view: proj,
                            world_from_object: conv::array4x4(transform),
                            view_from_world: conv::array4x4(view_transform),
                        };

                    let params = glium::DrawParameters {
                        depth: glium::Depth {
                            test: glium::DepthTest::IfLess,
                            write: true,
                            ..Default::default()
                        },
                        ..Default::default()
                    };
                    frame
                        .draw(&model.vertex, &model.index, &program, &uniforms, &params)
                        .unwrap();
                }
            }
        }
        app.host_mut().render_queue.clear();
        frame.finish().expect("Should render?");
    }
}

struct Input {
    w_pressed: ElementState,
    s_pressed: ElementState,
    a_pressed: ElementState,
    d_pressed: ElementState,
    mouse_x: f64,
    mouse_y: f64,
}

impl Input {
    fn new() -> Input {
        Input {
            w_pressed: ElementState::Released,
            s_pressed: ElementState::Released,
            a_pressed: ElementState::Released,
            d_pressed: ElementState::Released,
            mouse_x: 0.0,
            mouse_y: 0.0,
        }
    }

    fn handle_input(
        &mut self,
        events_loop: &mut glium::glutin::EventsLoop,
        app: &mut live_reload::Reloadable<Host>,
    ) -> bool {
        let mut running = true;
        app.host_mut().set_axis(Axis::LookLR, 0.0);
        app.host_mut().set_axis(Axis::LookUD, 0.0);
        events_loop.poll_events(|event| match event {
            Event::WindowEvent { event: WindowEvent::Closed, .. } => {
                running = false;
            }
            Event::WindowEvent { event: WindowEvent::Focused(is_focused), .. } => {
                if is_focused {
                    println!("FOCUS");
                } else {
                    println!("BLUR");
                }
            }
            Event::DeviceEvent {
                device_id,
                event: DeviceEvent::Motion { axis, value },
                ..
            } => {
                match axis {
                    0 => app.host_mut().set_axis(Axis::LookLR, value as f32),
                    1 => app.host_mut().set_axis(Axis::LookUD, value as f32),
                    _ => (),
                }
                // println!(
                //     "Device ID: {:?}, Axis: {}, Value: {}",
                //     device_id,
                //     axis,
                //     value
                // );
            }
            Event::WindowEvent { event: WindowEvent::KeyboardInput { input, .. }, .. } => {
                match input.virtual_keycode {
                    Some(VirtualKeyCode::W) => {
                        self.w_pressed = input.state;
                        if self.w_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveUD, 1.0);
                        } else if self.s_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveUD, -1.0);
                        } else {
                            app.host_mut().set_axis(Axis::MoveUD, 0.0);
                        }
                    }
                    Some(VirtualKeyCode::S) => {
                        self.s_pressed = input.state;
                        if self.s_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveUD, -1.0);
                        } else if self.w_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveUD, 1.0);
                        } else {
                            app.host_mut().set_axis(Axis::MoveUD, 0.0);
                        }
                    }
                    Some(VirtualKeyCode::A) => {
                        self.a_pressed = input.state;
                        if self.a_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveLR, -1.0);
                        } else if self.d_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveLR, 1.0);
                        } else {
                            app.host_mut().set_axis(Axis::MoveLR, 0.0);
                        }
                    }
                    Some(VirtualKeyCode::D) => {
                        self.d_pressed = input.state;
                        if self.d_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveLR, 1.0);
                        } else if self.a_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveLR, -1.0);
                        } else {
                            app.host_mut().set_axis(Axis::MoveLR, 0.0);
                        }
                    }
                    Some(other) => println!("{:?} {:?}?", other, input.state),
                    None => println!("None {:?}???", input.state),
                }
            }
            // evt => println!("{:?}", evt),
            _ => (),
        });
        running
    }
}
