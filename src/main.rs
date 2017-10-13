#[macro_use]
extern crate glium;
extern crate cgmath;
extern crate live_reload;

use live_reload::ShouldQuit;
use glium::Surface;
use glium::glutin::{Event, WindowEvent, VirtualKeyCode, ElementState};
use cgmath::prelude::*;

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

#[derive(Copy, Clone)]
struct Vert {
    pos: [f32; 3],
    norm: [f32; 3],
}

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

implement_vertex!(Vert, pos);

pub fn main() {
    let mut events_loop = glium::glutin::EventsLoop::new();
    let window = glium::glutin::WindowBuilder::new()
        .with_dimensions(1280, 720)
        .with_title("Garden");
    let context = glium::glutin::ContextBuilder::new().with_vsync(true);
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
    let program = glium::program::Program::from_source(
        &display,
        "
#version 410
in vec3 pos;
uniform mat4 proj;
uniform mat4 model;
void main() {
  gl_Position = proj * model * vec4(pos, 1);
}
",
        "
#version 410
out vec4 out_color;
uniform vec4 color;
void main() {
  out_color = color;
}
",
        None,
    ).unwrap();

    let mut running = true;
    let mut w_pressed = ElementState::Released;
    let mut s_pressed = ElementState::Released;
    let mut a_pressed = ElementState::Released;
    let mut d_pressed = ElementState::Released;
    while running {
        app.reload().expect("Should safely reload!");

        events_loop.poll_events(|event| match event {
            Event::WindowEvent { event: WindowEvent::Closed, .. } => {
                running = false;
            }
            Event::WindowEvent { event: WindowEvent::KeyboardInput { input, .. }, .. } => {
                match input.virtual_keycode {
                    Some(VirtualKeyCode::W) => {
                        w_pressed = input.state;
                        if w_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveUD, 1.0);
                        } else if s_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveUD, -1.0);
                        } else {
                            app.host_mut().set_axis(Axis::MoveUD, 0.0);
                        }
                    }
                    Some(VirtualKeyCode::S) => {
                        s_pressed = input.state;
                        if s_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveUD, -1.0);
                        } else if w_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveUD, 1.0);
                        } else {
                            app.host_mut().set_axis(Axis::MoveUD, 0.0);
                        }
                    }
                    Some(VirtualKeyCode::A) => {
                        a_pressed = input.state;
                        if a_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveLR, -1.0);
                        } else if d_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveLR, 1.0);
                        } else {
                            app.host_mut().set_axis(Axis::MoveLR, 0.0);
                        }
                    }
                    Some(VirtualKeyCode::D) => {
                        d_pressed = input.state;
                        if d_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveLR, 1.0);
                        } else if a_pressed == ElementState::Pressed {
                            app.host_mut().set_axis(Axis::MoveLR, -1.0);
                        } else {
                            app.host_mut().set_axis(Axis::MoveLR, 0.0);
                        }
                    }
                    Some(other) => println!("{:?} {:?}?", other, input.state),
                    None => println!("None {:?}???", input.state),
                }
            }
            _ => (),
            // evt => println!("{:?}", evt),
        });

        if app.update() == ShouldQuit::Yes {
            running = false;
        }

        let mut frame = display.draw();
        frame.clear_color(0.0, 0.0, 0.0, 1.0);

        let uniforms =
            uniform!{
                proj: cgmath::conv::array4x4(cgmath::perspective(
                    cgmath::Deg(80.0f32),
                    1280.0 / 720.0,
                    0.1,
                    500.0,
                )),
                model: cgmath::conv::array4x4(cgmath::Matrix4::from_translation(cgmath::Vector3 { x: app.host().pos[0], y: app.host().pos[1], z: app.host().pos[2] })
                ),
                color: app.host().clear_color,
            };

        frame
            .draw(
                &model.vertex,
                &model.index,
                &program,
                &uniforms,
                &Default::default(),
            )
            .unwrap();
        frame.finish().expect("Should render?");
    }
}
