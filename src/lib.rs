#[macro_use]
extern crate live_reload;

mod host;
use host::{Axis, Host};

struct State {}

live_reload! {
    host: Host;
    state: State;
    init: init;
    reload: reload;
    update: update;
    unload: unload;
    deinit: deinit;
}

fn init(_host: &mut Host, _state: &mut State) {}
fn reload(_host: &mut Host, _state: &mut State) {}
fn update(host: &mut Host, _state: &mut State) -> live_reload::ShouldQuit {
    host.clear_color[0] = (host.get_axis(Axis::MoveLR) + 1.0) / 2.0;
    host.clear_color[1] = (host.get_axis(Axis::MoveUD) + 1.0) / 2.0;
    host.clear_color[2] = 0.0;
    live_reload::ShouldQuit::No
}
fn unload(_host: &mut Host, _state: &mut State) {}
fn deinit(_host: &mut Host, _state: &mut State) {}
