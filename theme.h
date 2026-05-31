#include <QString>
#include <QFile>
#include <QTextStream>

namespace Theme
{
inline QString load()
{
	QFile file(":/main.qss");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return "";
	}

	QString qss = QTextStream(&file).readAll();
	qss.replace("{{bg}}", "#24273a"); // base
	qss.replace("{{mantle}}", "#1e2030"); // mantle
	qss.replace("{{fg}}", "#cad3f5"); // text
	qss.replace("{{muted}}", "#a5adcb"); // muted
	qss.replace("{{border}}", "#363a4f"); // surface
	qss.replace("{{hover}}", "#363a4f"); // surface
	qss.replace("{{active}}", "#6e738d"); // overlay
	qss.replace("{{primary}}", "#f5bde6"); // pink

	return qss;
}
}
