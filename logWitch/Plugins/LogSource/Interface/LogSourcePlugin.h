/*
 * LogSourcePlugin.h
 *
 *  Created on: Apr 18, 2019
 *      Author: sven
 */

#ifndef LOGWITCH_PLUGINS_LOGSOURCE_INTERFACE_LOGSOURCEPLUGIN_H_
#define LOGWITCH_PLUGINS_LOGSOURCE_INTERFACE_LOGSOURCEPLUGIN_H_
#include <QString>
#include <QObject>
#include <QToolBar>

namespace logwitch {
	class ParserActionInterface;

	namespace plugins {

	class LogSourcePluginDesription
	{
	public:
		QString name;
		QString description;
		QString version;
	};

	/**
	 * This is a interface for making a plugin which may act as a source for LogEntires
	 * including the GUI elements and view specific behaviors. There a re some types
	 * of sources which may differ in their behavior and how they are used.
	 */
	class LogSourcePlugin
	{
	public:
		virtual ~LogSourcePlugin () = default;

		/**
		 * Attaches the parser action interface which needs to be done before doing
		 * anything else with the plugin.
		 */
		virtual void attachParserAction( logwitch::ParserActionInterface* parserActionIfc ) = 0;

  	/**
		 * Returns the plugin description.
		 */
		virtual const LogSourcePluginDesription& getDescription() const = 0;

		/**
		 * Returns a toolbar which shall be added to support initiating the source if
		 * there are some quick-access things like profiles ports or similar selections.
		 * The Toolbar has no connection to the already instantiated receiver and thus
		 * can support them only till the were created.
		 *
		 * @return The toolbar or null_ptr if there is no toolbar to display.
		 */
		virtual QToolBar* getToolbar() = 0;

		/**
		 * The plugin will fill this menu with custom actions or something else. The
		 * menu given is located in the "Log Source" menu. The plugin can also do
		 * nothing if it is not necessary.
		 */
		virtual void fillMenu( QMenu* menu ) = 0;
	};
}}

Q_DECLARE_INTERFACE(logwitch::plugins::LogSourcePlugin,
										"de.steckmann.logwitch.plugins.LogSourcePlugin/1.0");

namespace logwitch { namespace plugins {
	/**
	 * This is a convenience implementation which does some common stuff for the
	 * implementer.
	 */
	class LogSourcePluginAbstractBase: public LogSourcePlugin
	{
		Q_INTERFACES(logwitch::plugins::LogSourcePlugin)

	public:
		LogSourcePluginAbstractBase();

		const LogSourcePluginDesription& getDescription() const override { return m_pluginDescription; }

		QToolBar* getToolbar() override { return nullptr; }

		void fillMenu(QMenu* menu) override { }

		void attachParserAction(logwitch::ParserActionInterface* parserActionIfc) override { m_parserActionIfc = parserActionIfc; }

	protected:
		LogSourcePluginDesription m_pluginDescription;

		logwitch::ParserActionInterface* m_parserActionIfc;
	};

}}

#endif /* LOGWITCH_PLUGINS_LOGSOURCE_INTERFACE_LOGSOURCEPLUGIN_H_ */
