program main
        use pbar_type
		use logmmap
		
		type(pbar_t) :: barra
        integer(8) :: ii, jj, NN=1000000, el=20, NTH=1
        character (len=20) :: nome, fill
        nome='reso.log'
        fill='|'
        
        call pbar_init(barra, nome, NN, el, NTH, fill)
        do ii = 1,NN
                call pbar_draw(barra, ii)
                do jj =1,100000
                enddo
        enddo
        call pbar_close(barra)
end program main
