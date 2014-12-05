
	int n0, n1, n2, n3, n4, n5, n6, n7;
	int m0, m1, m2, m3, m4, m5, m6, m7;
	n0 = oar.tell();  oar << v1;  n1 = oar.tell() - n0;
	n0 = oar.tell();  oar << v2;  n2 = oar.tell() - n0;
	n0 = oar.tell();  oar << v3;  n3 = oar.tell() - n0;
	n0 = oar.tell();  oar << v4;  n4 = oar.tell() - n0;
	n0 = oar.tell();  oar << v5;  n5 = oar.tell() - n0;
	n0 = oar.tell();  oar << v6;  n6 = oar.tell() - n0;
	n0 = oar.tell();  oar << arr; n7 = oar.tell() - n0;

	iar.set(oar.begin(), oar.current());
	w1.clear();
	w2.clear();
	w3.clear();
	w4.clear();
	w5.clear();
	w6.clear();
	m0 = iar.diff(oar.begin());  iar >> w1;  m1 = iar.diff(oar.begin()) - m0;
	m0 = iar.diff(oar.begin());  iar >> w2;  m2 = iar.diff(oar.begin()) - m0;
	m0 = iar.diff(oar.begin());  iar >> w3;  m3 = iar.diff(oar.begin()) - m0;
	m0 = iar.diff(oar.begin());  iar >> w4;  m4 = iar.diff(oar.begin()) - m0;
	m0 = iar.diff(oar.begin());  iar >> w5;  m5 = iar.diff(oar.begin()) - m0;
	m0 = iar.diff(oar.begin());  iar >> w6;  m6 = iar.diff(oar.begin()) - m0;
	m0 = iar.diff(oar.begin());  iar >> arr2;m7 = iar.diff(oar.begin()) - m0;

	assert(iar.eof());

	assert(m1 == n1);
	assert(m2 == n2);
	assert(m3 == n3);
	assert(m4 == n4);
	assert(m5 == n5);
	assert(m6 == n6);
	assert(m7 == n7);

	assert(v1.size() == w1.size());
	assert(v2.size() == w2.size());
	assert(v3.size() == w3.size());
	assert(v4.size() == w4.size());
	assert(v5.size() == w5.size());
	assert(v6.size() == w6.size());

	memcmp(&count, &count, 4);

	for (size_t i = 0, n = v1.size(); i < n; ++i) assert(Equal()(v1[i], w1[i]));
	for (size_t i = 0, n = v2.size(); i < n; ++i) assert(Equal()(v2[i], w2[i]));
	for (size_t i = 0, n = v3.size(); i < n; ++i) assert(Equal()(v3[i], w3[i]));
	for (size_t i = 0, n = v4.size(); i < n; ++i) assert(Equal()(v4[i], w4[i]));
	for (size_t i = 0, n = v5.size(); i < n; ++i) assert(Equal()(v5[i], w5[i]));
	for (size_t i = 0, n = v6.size(); i < n; ++i) assert(Equal()(v6[i], w6[i]));
	for (size_t i = 0; i < sizeof(arr)/sizeof(arr[0]); ++i) assert(Equal()(arr[i], arr2[i]));
