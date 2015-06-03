    
		% Scene object types
        sim_object_shape_type           =0;
		sim_object_joint_type           =1;
		sim_object_graph_type           =2;
		sim_object_camera_type          =3;
		sim_object_dummy_type           =4;
		sim_object_proximitysensor_type =5;
		sim_object_reserved1            =6;
		sim_object_reserved2            =7;
		sim_object_path_type            =8;
		sim_object_visionsensor_type    =9;
		sim_object_volume_type          =10;
		sim_object_mill_type            =11;
		sim_object_forcesensor_type     =12;
		sim_object_light_type           =13;
		sim_object_mirror_type          =14;

		%General object types
		sim_appobj_object_type          =109;
		sim_appobj_collision_type       =110;
		sim_appobj_distance_type        =111;
		sim_appobj_simulation_type      =112;
		sim_appobj_ik_type              =113;
		sim_appobj_constraintsolver_type=114;
		sim_appobj_collection_type      =115;
		sim_appobj_ui_type              =116;
		sim_appobj_script_type          =117;
		sim_appobj_pathplanning_type    =118;
		sim_appobj_RESERVED_type        =119;
		sim_appobj_texture_type         =120;

		% Ik calculation methods
		sim_ik_pseudo_inverse_method        =0;
		sim_ik_damped_least_squares_method  =1;
		sim_ik_jacobian_transpose_method    =2;

		% Ik constraints
		sim_ik_x_constraint         =1;
		sim_ik_y_constraint         =2;
		sim_ik_z_constraint         =4;
		sim_ik_alpha_beta_constraint=8;
		sim_ik_gamma_constraint     =16;
		sim_ik_avoidance_constraint =64;

		% Ik calculation results 
		sim_ikresult_not_performed  =0;
		sim_ikresult_success        =1;
		sim_ikresult_fail           =2;

		% Scene object sub-types 
		sim_light_omnidirectional_subtype   =1;
		sim_light_spot_subtype              =2;
		sim_light_directional_subtype       =3;
		sim_joint_revolute_subtype          =10;
		sim_joint_prismatic_subtype         =11;
		sim_joint_spherical_subtype         =12;
		sim_shape_simpleshape_subtype       =20;
		sim_shape_multishape_subtype        =21;
		sim_proximitysensor_pyramid_subtype =30;
		sim_proximitysensor_cylinder_subtype=31;
		sim_proximitysensor_disc_subtype    =32;
		sim_proximitysensor_cone_subtype    =33;
		sim_proximitysensor_ray_subtype     =34;
		sim_mill_pyramid_subtype            =40;
		sim_mill_cylinder_subtype           =41;
		sim_mill_disc_subtype               =42;
		sim_mill_cone_subtype               =42;
		sim_object_no_subtype               =200;


		%Scene object main properties
		sim_objectspecialproperty_collidable					=1;
		sim_objectspecialproperty_measurable					=2;
		sim_objectspecialproperty_detectable_ultrasonic			=16;
		sim_objectspecialproperty_detectable_infrared			=32;
		sim_objectspecialproperty_detectable_laser				=64;
		sim_objectspecialproperty_detectable_inductive			=128;
		sim_objectspecialproperty_detectable_capacitive			=256;
		sim_objectspecialproperty_renderable					=512;
		sim_objectspecialproperty_detectable_all				=496;
		sim_objectspecialproperty_cuttable						=1024;
		sim_objectspecialproperty_pathplanning_ignored			=2048;

		% Model properties
		sim_modelproperty_not_collidable				=1;
		sim_modelproperty_not_measurable				=2;
		sim_modelproperty_not_renderable				=4;
		sim_modelproperty_not_detectable				=8;
		sim_modelproperty_not_cuttable					=16;
		sim_modelproperty_not_dynamic					=32;
		sim_modelproperty_not_respondable				=64;
		sim_modelproperty_not_reset						=128;
		sim_modelproperty_not_visible					=256;
		sim_modelproperty_not_model						=61440;


		% Check the documentation instead of comments below!! 
		sim_message_ui_button_state_change  =0;	
		sim_message_reserved9               =1;	
		sim_message_object_selection_changed=2;
		sim_message_reserved10				=3;	
		sim_message_model_loaded            =4;
		sim_message_reserved11				=5;	
		sim_message_keypress				=6;	
		sim_message_bannerclicked			=7;	
		sim_message_for_c_api_only_start        =256;  	
		sim_message_reserved1                   =257;  	
		sim_message_reserved2			        =258;  	
		sim_message_reserved3			        =259;  	
		sim_message_eventcallback_scenesave		=260;		
		sim_message_eventcallback_modelsave		=261;      
		sim_message_eventcallback_moduleopen	=262;	    
		sim_message_eventcallback_modulehandle	=263;	    
		sim_message_eventcallback_moduleclose	=264;	    
		sim_message_reserved4					=265;      
		sim_message_reserved5					=266;	    
		sim_message_reserved6					=267;	    
		sim_message_reserved7					=268;	    
		sim_message_eventcallback_instancepass	=269;		
		sim_message_eventcallback_broadcast     =270;
		sim_message_eventcallback_imagefilter_enumreset =271;
		sim_message_eventcallback_imagefilter_enumerate      =272;
		sim_message_eventcallback_imagefilter_adjustparams   =273;
		sim_message_eventcallback_imagefilter_reserved       =274;
		sim_message_eventcallback_imagefilter_process        =275;
		sim_message_eventcallback_reserved1                  =276;  
		sim_message_eventcallback_reserved2                  =277;   
		sim_message_eventcallback_reserved3                  =278;   
		sim_message_eventcallback_reserved4                  =279;   
		sim_message_eventcallback_abouttoundo		         =280;   
		sim_message_eventcallback_undoperformed	             =281;   
		sim_message_eventcallback_abouttoredo		         =282;   
		sim_message_eventcallback_redoperformed	             =283;   
		sim_message_eventcallback_scripticondblclick         =284;   
		sim_message_eventcallback_simulationabouttostart     =285;
		sim_message_eventcallback_simulationended            =286;
		sim_message_eventcallback_reserved5			         =287;   
		sim_message_eventcallback_keypress			         =288;   
		sim_message_eventcallback_modulehandleinsensingpart  =289;   
		sim_message_eventcallback_renderingpass              =290;   
		sim_message_eventcallback_bannerclicked              =291;   
		sim_message_eventcallback_menuitemselected           =292;   
		sim_message_eventcallback_refreshdialogs             =293;   
		sim_message_eventcallback_sceneloaded                =294;
		sim_message_eventcallback_modelloaded                =295;
		sim_message_eventcallback_instanceswitch             =296;
		sim_message_eventcallback_guipass                    =297;
		sim_message_eventcallback_mainscriptabouttobecalled  =298;
		sim_message_eventcallback_rmlposition                =299;   
		sim_message_eventcallback_rmlvelocity                =300;
		   
		sim_message_simulation_start_resume_request          =4096;
		sim_message_simulation_pause_request                 =4097;
		sim_message_simulation_stop_request                  =4098;

		% Scene object properties 
		sim_objectproperty_collapsed				=16;
		sim_objectproperty_selectable				=32;
		sim_objectproperty_reserved7				=64;
		sim_objectproperty_selectmodelbaseinstead	=128;
		sim_objectproperty_dontshowasinsidemodel	=256;
		


		% type of arguments (input and output) for custom lua commands 
		sim_lua_arg_nil     =0;
		sim_lua_arg_bool	=1;	
		sim_lua_arg_int     =2;
		sim_lua_arg_float   =3;
		sim_lua_arg_string  =4;
		sim_lua_arg_invalid =5;
		sim_lua_arg_table   =8;

		% custom user interface properties 
		sim_ui_property_visible						=1;
		sim_ui_property_visibleduringsimulationonly	=2;
		sim_ui_property_moveable					=4;
		sim_ui_property_relativetoleftborder		=8;
		sim_ui_property_relativetotopborder			=16;
		sim_ui_property_fixedwidthfont				=32;
		sim_ui_property_systemblock					=64;
		sim_ui_property_settocenter					=128;
		sim_ui_property_rolledup					=256;
		sim_ui_property_selectassociatedobject		=512;
		sim_ui_property_visiblewhenobjectselected	=1024;


		% button properties 
		sim_buttonproperty_button				=0;
		sim_buttonproperty_label				=1;
		sim_buttonproperty_slider				=2;
		sim_buttonproperty_editbox				=3;
		sim_buttonproperty_staydown				=8;
		sim_buttonproperty_enabled				=16;
		sim_buttonproperty_borderless			=32;
		sim_buttonproperty_horizontallycentered	=64;
		sim_buttonproperty_ignoremouse			=128;
		sim_buttonproperty_isdown				=256;
		sim_buttonproperty_transparent			=512;
		sim_buttonproperty_nobackgroundcolor	=1024;
		sim_buttonproperty_rollupaction			=2048;
		sim_buttonproperty_closeaction			=4096;
		sim_buttonproperty_verticallycentered	=8192;
		sim_buttonproperty_downupevent			=16384;


		% Simulation status 
		sim_simulation_stopped						=0;								
		sim_simulation_paused						=8;								
		sim_simulation_advancing					=16;								
		sim_simulation_advancing_firstafterstop		=16;		
		sim_simulation_advancing_running			=17;		
		sim_simulation_advancing_lastbeforepause	=19;		
		sim_simulation_advancing_firstafterpause	=20;		
		sim_simulation_advancing_abouttostop		=21;		
		sim_simulation_advancing_lastbeforestop		=22;		


		% Script execution result (first return value) 
		sim_script_no_error					=0;
		sim_script_main_script_nonexistent	=1;
		sim_script_main_script_not_called	=2;
		sim_script_reentrance_error			=4;
		sim_script_lua_error				=8;
		sim_script_call_error				=16;


 		% Script types 
		sim_scripttype_mainscript   =0;
		sim_scripttype_childscript  =1;
		sim_scripttype_pluginscript =2;
		sim_scripttype_threaded     =240;			

		% API call error messages 
		sim_api_errormessage_ignore	=0;	
		sim_api_errormessage_report	=1;	
		sim_api_errormessage_output	=2;  


		% special argument of some functions 
		sim_handle_all						=-2;
		sim_handle_all_except_explicit		=-3;
		sim_handle_self						=-4;
		sim_handle_main_script				=-5;
		sim_handle_tree						=-6;
		sim_handle_chain					=-7;
		sim_handle_single					=-8;
		sim_handle_default					=-9;
		sim_handle_all_except_self			=-10;
		sim_handle_parent					=-11;


		% distance calculation methods 
		sim_distcalcmethod_dl               =0;
		sim_distcalcmethod_dac              =1;
		sim_distcalcmethod_max_dl_dac       =2;
		sim_distcalcmethod_dl_and_dac       =3;
		sim_distcalcmethod_sqrt_dl2_and_dac2=4;
		sim_distcalcmethod_dl_if_nonzero    =5;
		sim_distcalcmethod_dac_if_nonzero   =6;


 		% Generic dialog styles 
		sim_dlgstyle_message		=0;
		sim_dlgstyle_input          =1;
		sim_dlgstyle_ok             =2;
		sim_dlgstyle_ok_cancel      =3;
		sim_dlgstyle_yes_no         =4;
		sim_dlgstyle_dont_center	=32;

 		% Generic dialog return values 
		sim_dlgret_still_open   =0;
		sim_dlgret_ok           =1;
		sim_dlgret_cancel       =2;
		sim_dlgret_yes          =3;
		sim_dlgret_no           =4;


		% Path properties 
		sim_pathproperty_show_line				            =1;
		sim_pathproperty_show_orientation		            =2;
		sim_pathproperty_closed_path			            =4;
		sim_pathproperty_automatic_orientation	            =8;
		sim_pathproperty_invert_velocity		            =16;
		sim_pathproperty_infinite_acceleration	            =32;
		sim_pathproperty_flat_path				            =64;
		sim_pathproperty_show_position			            =128;
		sim_pathproperty_auto_velocity_profile_translation	=256;
		sim_pathproperty_auto_velocity_profile_rotation		=512;
		sim_pathproperty_endpoints_at_zero					=1024;
		sim_pathproperty_keep_x_up							=2048;


 		% drawing objects 
		sim_drawing_points          =0;		
		sim_drawing_lines			=1;       	
		sim_drawing_triangles		=2;	        
		sim_drawing_trianglepoints	=3;	        
		sim_drawing_quadpoints		=4;	        
		sim_drawing_discpoints		=5;	        
		sim_drawing_cubepoints		=6;     	
		sim_drawing_spherepoints	=7;  		

		sim_drawing_itemcolors				=32; 
		sim_drawing_vertexcolors			=64; 
		sim_drawing_itemsizes				=128; 
		sim_drawing_backfaceculling			=256; 
		sim_drawing_wireframe				=512; 
		sim_drawing_painttag				=1024; 
		sim_drawing_followparentvisibility	=2048; 
		sim_drawing_cyclic					=4096; 
		sim_drawing_50percenttransparency	=8192; 
		sim_drawing_25percenttransparency	=16384; 
		sim_drawing_12percenttransparency	=32768; 
		sim_drawing_emissioncolor			=65536; 
		sim_drawing_facingcamera			=131072; 
		sim_drawing_overlay					=262144; 
		sim_drawing_itemtransparency		=524288;  

		% banner values 
		sim_banner_left						=1; 
		sim_banner_right					=2; 
		sim_banner_nobackground				=4; 
		sim_banner_overlay					=8; 
		sim_banner_followparentvisibility	=16; 
		sim_banner_clickselectsparent		=32; 
		sim_banner_clicktriggersevent		=64; 
		sim_banner_facingcamera				=128; 
		sim_banner_fullyfacingcamera		=256; 
		sim_banner_backfaceculling			=512; 
		sim_banner_keepsamesize				=1024; 
		sim_banner_bitmapfont				=2048; 

		% particle objects 
		sim_particle_points1        =0;  
		sim_particle_points2		=1;	
		sim_particle_points4		=2;	
		sim_particle_roughspheres	=3;	
		sim_particle_spheres		=4;	

		sim_particle_respondable1to4		=32; 
		sim_particle_respondable5to8		=64; 
		sim_particle_particlerespondable	=128; 
		sim_particle_ignoresgravity			=256; 
		sim_particle_invisible				=512; 
		sim_particle_itemsizes				=1024; 
		sim_particle_itemdensities			=2048; 
		sim_particle_itemcolors				=4096; 
		sim_particle_cyclic					=8192; 
		sim_particle_emissioncolor			=16384; 
		sim_particle_water					=32768; 
		sim_particle_painttag				=65536; 

		% custom user interface menu attributes 
		sim_ui_menu_title		=1;
		sim_ui_menu_minimize	=2;
		sim_ui_menu_close		=4;
		sim_ui_menu_systemblock =8;



		% Boolean parameters 
		sim_boolparam_hierarchy_visible                 =0;
		sim_boolparam_console_visible                   =1;
		sim_boolparam_collision_handling_enabled        =2;
		sim_boolparam_distance_handling_enabled         =3;
		sim_boolparam_ik_handling_enabled               =4;
		sim_boolparam_gcs_handling_enabled              =5;
		sim_boolparam_dynamics_handling_enabled         =6;
		sim_boolparam_joint_motion_handling_enabled     =7;
		sim_boolparam_path_motion_handling_enabled      =8;
		sim_boolparam_proximity_sensor_handling_enabled =9;
		sim_boolparam_vision_sensor_handling_enabled    =10;
		sim_boolparam_mill_handling_enabled             =11;
		sim_boolparam_browser_visible                   =12;
		sim_boolparam_scene_and_model_load_messages     =13;
		sim_reserved0                                   =14;
		sim_boolparam_shape_textures_are_visible        =15;
		sim_boolparam_display_enabled                   =16;
		sim_boolparam_infotext_visible                  =17;
		sim_boolparam_statustext_open                   =18;
		sim_boolparam_fog_enabled                       =19;
		sim_boolparam_rml2_available                    =20;
		sim_boolparam_rml4_available                    =21;
		sim_boolparam_mirrors_enabled					=22;
		sim_boolparam_aux_clip_planes_enabled			=23;
		sim_boolparam_full_model_copy_from_api			=24;
		sim_boolparam_realtime_simulation				=25;
		sim_boolparam_video_recording_triggered			=29;


		% Integer parameters 
		sim_intparam_error_report_mode      =0;  
		sim_intparam_program_version        =1;  
		sim_intparam_instance_count         =2;  
		sim_intparam_custom_cmd_start_id    =3;  
		sim_intparam_compilation_version    =4;  
		sim_intparam_current_page           =5;
		sim_intparam_flymode_camera_handle  =6;  
		sim_intparam_dynamic_step_divider   =7;  
		sim_intparam_dynamic_engine         =8;  
		sim_intparam_server_port_start      =9;  
		sim_intparam_server_port_range      =10; 
		sim_intparam_visible_layers         =11;
		sim_intparam_infotext_style         =12;
		sim_intparam_settings               =13;
		sim_intparam_edit_mode_type         =14;
		sim_intparam_server_port_next       =15; 
		sim_intparam_qt_version             =16; 
		sim_intparam_event_flags_read       =17; 
		sim_intparam_event_flags_read_clear =18; 
		sim_intparam_platform               =19; 
		sim_intparam_scene_unique_id        =20; 

		% Float parameters 
		sim_floatparam_rand=0;
		sim_floatparam_simulation_time_step=1; 

		% String parameters 
		sim_stringparam_application_path=0; 
		sim_stringparam_video_filename=1; 

		% Array parameters 
		sim_arrayparam_gravity          =0;
		sim_arrayparam_fog              =1;
		sim_arrayparam_fog_color        =2;
		sim_arrayparam_background_color1=3;
		sim_arrayparam_background_color2=4;
		sim_arrayparam_ambient_light    =5;

		% User interface elements 
		sim_gui_menubar						=1;
		sim_gui_popups						=2;
		sim_gui_toolbar1					=4;
		sim_gui_toolbar2					=8;
		sim_gui_hierarchy					=16;
		sim_gui_infobar						=32;
		sim_gui_statusbar					=64;
		sim_gui_scripteditor				=128;
		sim_gui_scriptsimulationparameters	=256;
		sim_gui_dialogs						=512;
		sim_gui_browser						=1024;
		sim_gui_all							=65535;


		% Joint modes 
		sim_jointmode_passive       =0;
		sim_jointmode_motion        =1;
		sim_jointmode_ik            =2;
		sim_jointmode_ikdependent   =3;
		sim_jointmode_dependent     =4;
		sim_jointmode_force         =5;


		% Navigation and selection modes with the mouse.
		sim_navigation_passive					=0;
		sim_navigation_camerashift				=1;
		sim_navigation_camerarotate				=2;
		sim_navigation_camerazoom				=3;
		sim_navigation_cameratilt				=4;
		sim_navigation_cameraangle				=5;
		sim_navigation_camerafly				=6;
		sim_navigation_objectshift				=7;
		sim_navigation_objectrotate				=8;
		sim_navigation_reserved2				=9;
		sim_navigation_reserved3				=10;
		sim_navigation_jointpathtest			=11;
		sim_navigation_ikmanip					=12;
		sim_navigation_objectmultipleselection	=13;
		
		sim_navigation_reserved4				=256;
		sim_navigation_clickselection			=512;
		sim_navigation_ctrlselection			=1024;
		sim_navigation_shiftselection			=2048;
		sim_navigation_camerazoomwheel			=4096;
		sim_navigation_camerarotaterightbutton	=8192;


		% Remote API message header structure 
		simx_headeroffset_crc           =0;	
		simx_headeroffset_version       =2;	
		simx_headeroffset_message_id    =3;	
		simx_headeroffset_client_time   =7;	
		simx_headeroffset_server_time   =11;	
		simx_headeroffset_scene_id      =15;
		simx_headeroffset_server_state  =17;	

		% Remote API command header 
		simx_cmdheaderoffset_mem_size       =0;	
		simx_cmdheaderoffset_full_mem_size  =4;	
		simx_cmdheaderoffset_pdata_offset0  =8;	
		simx_cmdheaderoffset_pdata_offset1  =10;	
		simx_cmdheaderoffset_cmd			=14;
		simx_cmdheaderoffset_delay_or_split =18;	
		simx_cmdheaderoffset_sim_time       =20;	
		simx_cmdheaderoffset_status         =24;	
		simx_cmdheaderoffset_reserved       =25;	


		% Regular operation modes 
		simx_opmode_oneshot				=0; 
		simx_opmode_oneshot_wait		=65536; 
		simx_opmode_continuous			=131072;  
		simx_opmode_streaming			=131072; 

		% Operation modes for heavy data 
		simx_opmode_oneshot_split		=196608;   
		simx_opmode_continuous_split	=262144;
		simx_opmode_streaming_split		=262144;	

		% Special operation modes 
		simx_opmode_discontinue			=327680;	
		simx_opmode_buffer				=393216;	
		simx_opmode_remove				=458752;	


		% Command error codes 
		simx_error_noerror					=0;
		simx_error_novalue_flag				=1;		
		simx_error_timeout_flag				=2;		
		simx_error_illegal_opmode_flag		=4;		
		simx_error_remote_error_flag		=8;		
		simx_error_split_progress_flag		=16;		
		simx_error_local_error_flag			=32;		
		simx_error_initialize_error_flag	=64;	