#pragma once

#include <QtQuick/QQuickImageProvider>
#include <map>

class ImageProvider : public QQuickImageProvider
{
public:
	ImageProvider()
		: QQuickImageProvider(QQuickImageProvider::Pixmap)
	{}

	void addPixmap(QString id, QPixmap pm) { m_images.insert_or_assign(std::move(id), std::move(id)); }

	QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override
	{
		auto const &pm = [this, &id]() -> QPixmap const &
			{
				if(m_images.contains(id)) return m_images.at(id);
				static QPixmap defaultPm{64, 64};
				defaultPm.fill(Qt::transparent);

				return defaultPm;
			}();

		if(size) *size = pm.size();
		
		return pm.scaled(
			requestedSize.width() > 0 ? requestedSize.width() : pm.width(), 
			requestedSize.height() > 0 ? requestedSize.height() : pm.height());
	}

private:
	std::map<QString, QPixmap> m_images;
};
