using MicroServer.Net.Http.Mvc.Controllers;
using MicroServer.Utilities;
using System;
using System.IO;
using System.Text;

namespace MicroServer.Net.Http.Mvc.ActionResults
{
	/// <summary>
	/// Redirect to another url or controller/action.
	/// </summary>
	public class RedirectResult : ActionResult
	{
		private readonly string _url;

		/// <summary>
		/// Initializes a new instance of the <see cref="RedirectResult"/> class.
		/// </summary>
		/// <param name="url">Uri to redirect to.</param>
		/// <remarks>
		/// Include "http(s)://" in Uri to redirect to another site.
		/// </remarks>
		public RedirectResult(string url)
		{
			if (StringUtility.IsNullOrEmpty(url)) {
				throw new ArgumentException("url");
			}

			_url = url;
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="RedirectResult"/> class.
		/// </summary>
		/// <param name="controllerName">Name of the controller.</param>
		/// <param name="actionName">Name of the action.</param>
		public RedirectResult(string controllerName, string actionName)
		{
			if (StringUtility.IsNullOrEmpty(controllerName)) {
				throw new ArgumentException("controllerName");
			}

			if (StringUtility.IsNullOrEmpty(actionName)) {
				throw new ArgumentException("actionName");
			}

			_url = "/" + controllerName + "/" + actionName;
		}

		/// <summary>
		/// Execute the response result.
		/// </summary>
		/// <param name="context">HTTP controller context</param>
		/// <remarks>Invoked by <see cref="ControllerFactory"/> to process the response.</remarks>
		public override void ExecuteResult(IControllerContext context)
		{
			if (!StringUtility.IsNullOrEmpty(_url)) {
				var body = Encoding.UTF8.GetBytes(
						@"<!DOCTYPE html><html><head><META http-equiv='refresh' content='0;URL=" + _url + @"'</head><body></body></html>");
				context.HttpContext.Response.Body = new MemoryStream(body);
			}
		}
	}
}