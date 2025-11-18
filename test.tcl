set ::GL(PROGRAM_DIR) [file normalize [file dirname [info script]]]

wm iconify .
wm withdraw .
wm geometry . 1200x850
wm title . "Tcl/Tk with OpenGL :)"

set statusbar_frame [ttk::frame .statusbar_frame -height 34]
pack $statusbar_frame -side bottom -fill both -padx 0 -pady 0

	set ::statusbar [ttk::label $statusbar_frame.statusbar -compound left]
	pack $::statusbar -fill both -expand 1 -side left

	pack [ttk::separator $statusbar_frame.separator -orient vertical] \
		-fill y -side left -padx 5 -pady 2

	set ::actionbar [ttk::label $statusbar_frame.actionbar -width 50 -compound left]
	pack $::actionbar -fill both -side left
	
	set sizegrip [ttk::sizegrip $statusbar_frame.sizegrip]
	pack $sizegrip -side right -anchor se
	
set ::opengl_frame [ttk::frame .opengl_frame -borderwidth 0]
pack $::opengl_frame -side top -fill both -expand 1

	set ::renderer_frame [ttk::frame $::opengl_frame.renderer_frame -width 300 -height 200 -takefocus 1]
	pack $::renderer_frame -fill both -expand 1

	p_create_render_window $::renderer_frame [winfo id $::renderer_frame]
	
	set w [winfo width $::renderer_frame]
	set h [winfo height $::renderer_frame]
	p_resize_render_window $w $h

#wm attribute . -toolwindow 1
#wm overrideredirect . 1
wm deiconify .
wm geometry . +100+100
update idletasks
		
set ::resizing 0

#Tell C when the window resizes
bind $::renderer_frame <Configure> {
	set ::w %w
	set ::h %h
	if { $::resizing == 0 } {
		set ::resizing 1
		after 100 { 
			p_resize_render_window $::w $::h 
			set ::resizing 0
		}
	}
}
	
bind $::renderer_frame <Map> {
	p_redraw
}

puts "OpenGL version ... $GL(GL_VERSION) ([llength $GL(GL_EXTENSIONS)] GL extensions)"
puts "OpenGL vendor ... $GL(GL_VENDOR)"
puts "OpenGL renderer ... $GL(GL_RENDERER)"
puts "GLSL version ... $GL(GLSL_VERSION)"

# GPU information
set free_size [expr $GL(FREE_GPU_MEMORY_KB) / 1024]
set total_size [expr $GL(TOTAL_GPU_MEMORY_KB) / 1024]
puts "GPU memory ... total: ${total_size}MB  free: ${free_size}MB"

