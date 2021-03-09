	! progressbar/logmmapf.f90
	! Stored and maintained at https://github.com/giambagiamba/progressbar
        !
        ! This program is free software; you can redistribute it and/or modify
        ! it under the terms of the GNU General Public License as published by
        ! the Free Software Foundation; either version 3 of the License, or (at
        ! your option) any later version.
        ! 
        ! This program is distributed in the hope that it will be useful, but
        ! WITHOUT ANY WARRANTY; without even the implied warranty of
        ! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        ! General Public License for more details.
        ! 
        ! You should have received a copy of the GNU General Public License
        ! along with this program; if not, write to the Free Software
        ! Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

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
                SUBROUTINE pbar_init(progbar, filename, N, el, NT, fill) BIND(C, NAME='p_init')
                        use pbar_type
                        IMPLICIT NONE
                        type(pbar_t) :: progbar
                        character :: filename
                        integer(c_long) :: N, el, NT
                        character :: fill
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
