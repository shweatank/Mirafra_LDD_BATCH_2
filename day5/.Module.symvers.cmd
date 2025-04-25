cmd_/home/mirafra/ldd/day5/Module.symvers := sed 's/\.ko$$/\.o/' /home/mirafra/ldd/day5/modules.order | scripts/mod/modpost -m -a  -o /home/mirafra/ldd/day5/Module.symvers -e -i Module.symvers   -T -
