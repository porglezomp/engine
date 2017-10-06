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

#[derive(Copy, Clone)]
struct Vert {
    pos: [f32; 3],
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

    let vertex_data = &[
        Vert { pos: [-1.0, 0.0, 0.0] },
        Vert { pos: [0.0, 1.0, 0.0] },
        Vert { pos: [1.0, 0.0, 0.0] },
    ];
    let index_data = &[0u16, 1, 2];

    let vertex_buffer = glium::vertex::VertexBuffer::new(&display, vertex_data).unwrap();
    let indices = glium::index::IndexBuffer::new(
        &display,
        glium::index::PrimitiveType::TrianglesList,
        index_data,
    ).unwrap();
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
                &vertex_buffer,
                &indices,
                &program,
                &uniforms,
                &Default::default(),
            )
            .unwrap();
        frame.finish().expect("Should render?");
    }
}
