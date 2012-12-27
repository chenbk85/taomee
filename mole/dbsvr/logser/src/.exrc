if &cp | set nocp | endif
map  <i{
map  >i{ 
map ,e :e =expand("%:p:h") . "/" 
map ,r :!mtags.sh
map ,m :make
map ,c :!
map ,a :A
map ,N :N
map ,n :n
map ,q :q!
nmap \ihn :IHN
nmap \is :IHS:A
nmap \ih :IHS
let s:cpo_save=&cpo
set cpo&vim
nmap gx <Plug>NetrwBrowseX
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#NetBrowseX(expand("<cWORD>"),0)
map <C-F12> :!ctags -R  --languages=c++ --c++-kinds=+p --fields=+iaS --extra=+q .
map <F3> j.
map <F2> \+c
inoremap <expr>  omni#cpp#maycomplete#Complete()
inoremap <expr> . omni#cpp#maycomplete#Dot()
inoremap <expr> : omni#cpp#maycomplete#Scope()
inoremap <expr> > omni#cpp#maycomplete#Arrow()
imap \ihn :IHN
imap \is :IHS:A
imap \ih :IHS
abbr #d #define
abbr #i #include
abbr #e *******************************************************************************/
abbr #b /******************************************************************************* 
abbr #l /*----------------------------------------------------------------------------*/
abbr #x Ëñ<xCSI>ÊúùÊñá 
abbr S SQLCODE
abbr E EXEC SQL
let &cpo=s:cpo_save
unlet s:cpo_save
set backspace=indent,eol,start
set cindent
set fileencodings=ucs-bom,utf-8,gb2312,big5,euc-jp,euc-kr,latin1
set guifont=Bitstream\ Vera\ Sans\ Mono\ 9
set helplang=cn
set hlsearch
set incsearch
set laststatus=2
set omnifunc=omni#cpp#complete#Main
set path=.,/usr/include,,,~/ser/include,~/include
set ruler
set shiftwidth=4
set smarttab
set tabstop=4
set tags=./tags,./TAGS,tags,TAGS,~/ser/tags,~/.vim/tags
" vim: set ft=vim :
