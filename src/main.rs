#[macro_use]
extern crate glium;
extern crate live_reload;

use live_reload::ShouldQuit;
use glium::Surface;
use glium::glutin::{Event, WindowEvent, VirtualKeyCode, ElementState};

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
void main() {
  gl_Position = vec4(pos, 1);
}
",
        "
#version 410
out vec4 color;
void main() {
  color = vec4(1.0, 1.0, 1.0, 1.0);
}
",
        None,
    ).unwrap();
    let uniforms = uniform!{};

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
        frame.clear_color(
            app.host().clear_color[0],
            app.host().clear_color[1],
            app.host().clear_color[2],
            app.host().clear_color[3],
        );

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
