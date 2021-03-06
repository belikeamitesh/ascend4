import gtk, pango, gobject, re

class UnitsDialog:

	def __init__(self,browser,T=None):
		"""create units browser for a atom type T, or None"""
		self.browser = browser;

		# GUI config
		self.browser.builder.add_objects_from_file(self.browser.glade_file, ["unitsdialog"])
		self.window = self.browser.builder.get_object("unitsdialog")
		self.typecombo = self.browser.builder.get_object("typecombo")
		self.dimensionlabel = self.browser.builder.get_object("dimensionlabel")
		self.unitsview = self.browser.builder.get_object("unitsview")
		self.applybutton = self.browser.builder.get_object("units_applybutton")

		self.applybutton.set_sensitive(False)

		self.window.set_transient_for(self.browser.window)

		self.browser.builder.connect_signals(self)

		self.units = self.browser.library.getUnits()
		self.realtypes = self.browser.library.getRealAtomTypes()

		if not len(self.realtypes):
			self.browser.reporter.reportError("No dimensioned atom types available yet (have you loaded a model yet?)")
			return
			#raise RuntimeError("no units available")

		# set up columns in the units view:
		_renderer0 = gtk.CellRendererToggle()
		_renderer0.set_radio(True)
		_renderer0.connect("toggled",self.unitsview_row_toggled)
		_col0 = gtk.TreeViewColumn("",_renderer0,active=0)
		self.unitsview.append_column(_col0)
	
		_renderer1 = gtk.CellRendererText()	
		_col1 = gtk.TreeViewColumn("Units", _renderer1, text=1, weight=3)
		self.unitsview.append_column(_col1)

		# value column: 'editable' set by column 3 of the model data.
		_renderer2 = gtk.CellRendererText()	
		_col2 = gtk.TreeViewColumn("Conversion", _renderer2, text=2)
		self.unitsview.append_column(_col2)

		self.changed = {}
		self.T = T
		if T is not None:
			if T.isRefinedReal():
				self.typecombo.child.set_text(str(T.getName()))
		self.update_typecombo()

	def unitsview_row_toggled(self,widget,path,*args):
		i = self.unitsview.get_model().get_iter_from_string(path)
		n = self.unitsview.get_model().get_value(i,1)
		j = self.unitsview.get_model().get_iter_first()
		while j is not None:
			self.unitsview.get_model().set_value(j,0,False)
			j = self.unitsview.get_model().iter_next(j)
		self.unitsview.get_model().set_value(i,0,True)
		self.changed[self.typecombo.get_active_text()]=n
		#self.browser.reporter.reportNote("Units for '%s' set to '%s'" % (self.typecombo.get_active_text(),n))
		self.update_applybutton()	

	def update_applybutton(self):
		can_apply = False
		#print "changed = ",self.changed
		for k,v in self.changed.iteritems():
			T = self.browser.library.findType(k)
			u = T.getPreferredUnits()
			if u is None or str(T.getPreferredUnits().getName()) != v:
				#print "CAN APPLY: for type '%s', pref units currently '%s', now selected '%s'" % (T.getName(),T.getPreferredUnits().getName(), v)
				can_apply = True
				break
		self.applybutton.set_sensitive(can_apply)

	def update_typecombo(self,text = None):
		m = gtk.ListStore(str)
		for t in self.realtypes:
			if not text or re.compile("^%s"%re.escape(text)).match(str(t.getName())):
				m.append([t.getName()])
		self.typecombo.set_model(m)
		if text and m.iter_n_children(None):
			self.typecombo.popup()
			self.typecombo.child.grab_focus()
		self.typecombo.set_text_column(0)

	def update_unitsview(self,T):
		m = gtk.ListStore(bool,str,str,int)
		if T is not None:
			d = T.getDimensions()
			up = T.getPreferredUnits()
			if up is None:
				print "no preferred units"
			else:
				print "preferred units =",up.getName()
		else:
			up = None
		for u in self.units:
			if T is None or u.getDimensions()==d:
				if up is None:
					selected = False
				else:
					selected = (u==up)
				weight = pango.WEIGHT_NORMAL
				if selected:
					weight = pango.WEIGHT_BOLD
				du = u.getDimensions().getDefaultUnits().getName()
				if str(du) == "1":
					du = ""
				m.append([selected,u.getName(),"%g %s" %(u.getConversion(),du),weight])
		self.unitsview.set_model(m)

	def on_typecombo_changed(self,widget,*args):
		s = widget.get_active_text()
		self.update_typecombo(s)
		#self.browser.reporter.reportNote("value changed to '%s'" % s)
		try:
			T = self.browser.library.findType(widget.get_active_text())
			dt = str(T.getDimensions())
			self.dimensionlabel.set_text(dt)
		except:
			T = None
			self.dimensionlabel.set_text("")
		self.update_unitsview(T)
		
	def run(self):
		_res = gtk.RESPONSE_APPLY
		while _res == gtk.RESPONSE_APPLY:
			_res = self.window.run()
			if _res == gtk.RESPONSE_APPLY or _res == gtk.RESPONSE_CLOSE:
				if _res == gtk.RESPONSE_CLOSE and not len(self.realtypes):
					break
				for k,v in self.changed.iteritems():
					self.browser.prefs.setPreferredUnits(k,v)
				self.changed = {}
				typename = self.typecombo.get_active_text()
				if typename:
					self.update_unitsview(self.browser.library.findType(typename))
				else:
					self.update_unitsview(None)
				self.browser.modelview.refreshtree()
				for _obs in self.browser.observers:
					if _obs.alive:
						_obs.units_refresh(self.T)
		self.window.hide()

