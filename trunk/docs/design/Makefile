
INSTALLED_DOCS = \
    design.xml



all: pdf html 

pdf: $(subst .xml,.pdf,$(wildcard *.xml)) $(subst .tex,.pdf,$(wildcard *.tex))
html: $(subst .xml,.html,$(wildcard *.xml))
clean:
	rm -f *.log *.pdf *.html 


%.pdf: %.xml
	docbook2pdf $*.xml

%.html: %.xml
	docbook2html -u $*.xml



.PHONY: all pdf html clean
