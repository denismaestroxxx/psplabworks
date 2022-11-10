struct header {

	int size;

	void serialize(char* data) {
		memcpy(data, this, sizeof(header));
	}

	void deserialize(char* data) {
		memcpy(this, data, sizeof(header));
	}
};