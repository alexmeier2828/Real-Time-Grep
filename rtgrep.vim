let g:rtgrep_grep_command = "rtgrep \"rg --vimgrep --color=always\""
let g:rtgrep_temp_file = "/tmp/rtgrep_output.txt"

function RealTimeGrep()
    " Check if we're in neovim or vim
    if has('nvim')
        " Use terminal for neovim
        execute 'terminal ' . g:rtgrep_grep_command
        startinsert
        autocmd TermClose <buffer> call ProcessRGrepResults()
    else
        " Use shell command for vim (interactive)
        let l:command_string = '!' . g:rtgrep_grep_command . ' > ' . g:rtgrep_temp_file
        execute l:command_string
        call ProcessFileResults(g:rtgrep_temp_file)
    endif
endfunction

function ProcessRGrepResults()
    " Get the terminal buffer number
    let term_bufnr = bufnr('%')
    
    " Get all lines from the terminal buffer
    let lines = getbufline(term_bufnr, 1, '$')
    
    call ProcessGrepOutput(lines)
endfunction

function ProcessFileResults(filepath)
    " Read results from temporary file
    if filereadable(a:filepath)
        let lines = readfile(a:filepath)
        call ProcessGrepOutput(lines)
        " Clean up temporary file
        call delete(a:filepath)
    else
        echo "No matches found"
    endif
endfunction

function ProcessGrepOutput(lines)
    " Filter out empty lines and terminal prompts/control sequences
    let filtered_lines = []
    for line in a:lines
        " Clean ANSI escape sequences
        let cleaned_line = substitute(line, '\e\[[0-9;]*m', '', 'g')
        
        " Skip empty lines and lines that don't look like grep results
        if cleaned_line =~ '^\s*$' || cleaned_line =~ '^[^:]*$'
            continue
        endif
        " Look for lines that match the vimgrep format (file:line:column:text)
        if cleaned_line =~ ':\d\+:\d\+:'
            call add(filtered_lines, cleaned_line)
        endif
    endfor
    
    if !empty(filtered_lines)
        cexpr filtered_lines
        copen
    else
        echo "No matches found"
    endif
endfunction

:command RealTimeGrep :call RealTimeGrep()
