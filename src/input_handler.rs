use live_reload;
use glium;
use glium::glutin::{Event, WindowEvent, VirtualKeyCode, ElementState, DeviceEvent};
use host::{Host, Axis};
use cgmath;


pub struct Input {
    w_pressed: ElementState,
    s_pressed: ElementState,
    a_pressed: ElementState,
    d_pressed: ElementState,
    mouse_x: f64,
    mouse_y: f64,
    // @Cleanup: Does this belong in here?
    window_width: u32,
    window_height: u32,
}

impl Input {
    pub fn new() -> Input {
        Input {
            w_pressed: ElementState::Released,
            s_pressed: ElementState::Released,
            a_pressed: ElementState::Released,
            d_pressed: ElementState::Released,
            mouse_x: 0.0,
            mouse_y: 0.0,
            window_width: 1280,
            window_height: 720,
        }
    }

    pub fn handle_input(
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
            Event::WindowEvent { event: WindowEvent::Resized(width, height), .. } => {
                self.window_width = width;
                self.window_height = height;
            }
            evt => println!("{:?}", evt),
            _ => (),
        });
        running
    }

    pub fn projection(&self) -> cgmath::Matrix4<f32> {
        cgmath::perspective(
            cgmath::Deg(80.0f32),
            self.window_width as f32 / self.window_height as f32,
            0.1,
            500.0,
        )
    }
}
