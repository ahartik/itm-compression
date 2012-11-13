default:doc.pdf
PLOTS=ex3_small.pdf ex3_big.pdf
$(PLOTS): %.pdf: %.dot
	dot -Tpdf $< > $@

doc.pdf:doc.tex $(PLOTS)
	pdflatex doc.tex
	pdflatex doc.tex
