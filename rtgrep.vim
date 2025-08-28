function RGrep()
    " Save current directory
    let saved_dir = getcwd()
    
    " Run rtgrep interactively in a terminal
    execute 'terminal rtgrep "rg --vimgrep"'
    
    " Wait for the terminal to close, then process results
    autocmd TermClose <buffer> call ProcessRGrepResults()
endfunction

function ProcessRGrepResults()
    " Get the terminal buffer number
    let term_bufnr = bufnr('%')
    
    " Get all lines from the terminal buffer
    let lines = getbufline(term_bufnr, 1, '$')
    
    " Filter out empty lines and terminal prompts/control sequences
    let filtered_lines = []
    for line in lines
        " Skip empty lines and lines that don't look like grep results
        if line =~ '^\s*$' || line =~ '^[^:]*$'
            continue
        endif
        " Look for lines that match the vimgrep format (file:line:column:text)
        if line =~ ':\d\+:\d\+:'
            call add(filtered_lines, line)
        endif
    endfor
    
    if !empty(filtered_lines)
        cexpr filtered_lines
        copen
    else
        echo "No matches found"
    endif
endfunction

:command RGrep :call RGrep()
