	module pbar_type
		use, intrinsic :: iso_c_binding, only : c_char, c_long, c_double
		type, bind(C) :: pbar_t
				character(c_char) :: bar
				integer(c_long) :: len, Num, max, perc, nblks, err, file
				real(c_double) :: start
				integer(c_long) :: NT
		end type pbar_t
	end module pbar_type
	
	module logmmap
		INTERFACE
                SUBROUTINE pbar_init(progbar, filename, N, el, NT) BIND(C, NAME='p_init')
                        use pbar_type
                        IMPLICIT NONE
                        type(pbar_t) :: progbar
                        character :: filename
                        integer(c_long) :: N, el, NT
                END SUBROUTINE pbar_init
        END INTERFACE
		INTERFACE
                SUBROUTINE pbar_draw(progbar, argi) BIND(C, NAME='p_draw')
                        use pbar_type
                        IMPLICIT NONE
                        type(pbar_t) :: progbar
                        integer(c_long) :: argi
                END SUBROUTINE pbar_draw
        END INTERFACE
        INTERFACE
                SUBROUTINE pbar_close(progbar) BIND(C, NAME='pbar_close')
                        use pbar_type
                        IMPLICIT NONE
                        type(pbar_t) :: progbar
                END SUBROUTINE pbar_close
        END INTERFACE
    end module logmmap
