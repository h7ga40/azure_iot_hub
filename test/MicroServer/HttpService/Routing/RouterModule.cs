using MicroServer.Logging;
using MicroServer.Net.Http.Modules;
using MicroServer.Net.Http.Mvc.Resolver;
using MicroServer.Utilities;
using System;

namespace MicroServer.Net.Http.Routing
{
	/// <summary>
	/// Used to route the request based on <see cref="Regex"/> patterns.
	/// </summary>
	/// <remarks>Rewrites the request URI based on the <see cref="RouteConfig"/>.</remarks>
	public class RouterModule : IRoutingModule
	{
		private readonly RouteCollection _routes = new RouteCollection();

		/// <summary>
		/// Initializes a new instance of the <see cref="RouterModule"/> class.
		/// </summary>
		public RouterModule()
		{
			Invoke(_routes);
			LogRoutes();
		}

		/// <summary>
		/// Log the loaded routes.
		/// </summary>
		private void LogRoutes()
		{
			Logger.WriteInfo(this, "Ignored routes loaded from RouteConfig (HttpApplication) class:");
			foreach (string pattern in _routes.IngoredRoutes) {
				Logger.WriteInfo("  route regex: '" + pattern + "'");
			}

			Logger.WriteInfo(this, "Mapped routes loaded from RouteConfig (HttpApplication) class:");
			foreach (MappedRoute items in _routes.MappedRoutes) {
				var item = items;
				var route = item.defaults;

				Logger.WriteInfo("  route name: '" + item.name +
					"' regex: '" + item.regex +
					"' => default: {controller='" + route.controller + "' action='" + route.action + "' id='" + route.id + "'}"
					);
			}
		}

		/// <summary>
		/// Invoke the <see cref="RegisterRoutes"/> method in <see cref="RouteConfig"/>.
		/// </summary>
		/// <param name="routes">Routes.</param>
		private void Invoke(RouteCollection routes)
		{
			var configType = typeof(HttpApplication);

			var routeType = Find(configType);

			if (routeType != null) {
				var routeConfig = (HttpApplication)ServiceResolver.Current.Resolve(routeType);

				if (routes != null)
					routeConfig.RegisterRoutes(routes);
			}
		}

		/// <summary>
		/// Find the type in the current domain assemblies.
		/// </summary>
		/// <param name="configType">Routes</param>
		private Type Find(Type configType)
		{
			foreach (var assembly in AppDomain.CurrentDomain.GetAssemblies()) {
				foreach (var type in assembly.GetTypes()) {
					//Debug.Print(type.ToString());
					if (type.IsAbstract || type.IsInterface)
						continue;

					if (type.IsSubclassOf(configType))
						return type;
				}
			}

			return null;
		}

		#region IRoutingModule Members

		/// <summary>
		/// Invoked before anything else
		/// </summary>
		/// <param name="context">HTTP context</param>
		/// <remarks>
		/// <para>The first method that is executed in the pipeline.</para>
		/// Try to avoid throwing exceptions if you can. Let all modules have a chance to handle this method. You may break the processing in any other method than the Begin/EndRequest methods.</remarks>
		public void BeginRequest(IHttpContext context)
		{
		}

		/// <summary>
		/// Route the request.
		/// </summary>
		/// <param name="context">HTTP context</param>
		/// <returns><see cref="ModuleResult.Stop"/> will stop all processing including <see cref="IHttpModule.EndRequest"/>.</returns>
		/// <remarks>Simply change the request URI to something else.</remarks>
		public ModuleResult Route(IHttpContext context)
		{
			var results = _routes.Match(context);

			if (results != null) {
				if (results.MatchStatus.Success) {
					var defaultRoute = results.MappedRoute.defaults;

					var path = String.Empty;

					if (!StringUtility.IsNullOrEmpty(defaultRoute.controller))
						path += String.Concat("/", defaultRoute.controller);

					if (!StringUtility.IsNullOrEmpty(defaultRoute.action))
						path += String.Concat("/", defaultRoute.action);

					if (!StringUtility.IsNullOrEmpty(defaultRoute.id))
						path += String.Concat("?id=", defaultRoute.id);

					if (StringUtility.IsNullOrEmpty(path))
						path = context.Request.Uri.AbsolutePath;

					var url = context.Request.Uri;
					var port = String.Empty;

					if (url.Port != 80)
						port = String.Concat(";", url.Port);

					context.Request.UriRewrite = new Uri(
						String.Concat(url.Scheme, "://", url.Host, port, path));

				}
				return ModuleResult.Continue;
			}

			context.Response.StatusCode = 403;
			context.Response.StatusDescription = "Forbidden";
			return ModuleResult.Stop;
		}

		/// <summary>
		/// End request is typically used for post processing. The response should already contain everything required.
		/// </summary>
		/// <param name="context">HTTP context</param>
		/// <remarks>
		/// <para>The last method that is executed in the pipeline.</para>
		/// Try to avoid throwing exceptions if you can. Let all modules have a chance to handle this method. You may break the processing in any other method than the Begin/EndRequest methods.</remarks>
		public void EndRequest(IHttpContext context)
		{
		}

		#endregion
	}
}