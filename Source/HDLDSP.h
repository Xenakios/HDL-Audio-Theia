/*
todo:
	features:
		autogain
		oversampling
	bugs:
		drive kann zu extrem klingen
			drive verringern oder anders verrechnen?
*/

#pragma once

class HDLDSP {
public:
	enum Modes {
		ringModulatorMode,
		ampModulatorMode
	};

	HDLDSP() : m_bypassParam(-1), m_driveParam(1.f), m_mixParam(1.f){}

	void setParameters(float bypass, float driveSC, float mix) {
		m_bypassParam = int(bypass);
		m_mixParam = mix;

		if (m_driveParam != driveSC) {
			m_driveParam = driveSC;
			m_driveValue = m_driveParam * 7.f + 1.f;
		}
	}

	void process(float& sample, const float& scSample) {
		if (!m_bypassParam)
			sample = mix(sample, std::tanh(m_driveValue * sample) * std::tanh(m_driveValue * scSample), m_mixParam);
	}

private:
	static inline float mix(float a, float b, float p) { return a + p * (b - a); }

	int m_bypassParam;
	float m_driveParam, m_mixParam;
	float m_driveValue, m_outGainValue;
};