using System;
using System.Net;

namespace MicroServer.Net.Http
{
	/// <summary>
	/// Represents a HTTP request
	/// </summary>
	public interface IHttpRequest : IHttpMessage
	{
		/// <summary>
		///     Method which was invoked.
		/// </summary>
		/// <remarks>
		///     <para>Typically <c>GET</c>, <c>POST</c>, <c>PUT</c>, <c>DELETE</c> or <c>HEAD</c>.</para>
		/// </remarks>
		string HttpMethod { get; }

		/// <summary>
		///     Request UrI
		/// </summary>
		/// <remarks>
		///     <para>Is built using the <c>server</c> header and the path + query which is included in the request line</para>
		///     <para>If no <c>server</c> header is included "127.0.0.1" will be used as server.</para>
		/// </remarks>
		Uri Uri { get; set; }

		/// <summary>
		/// Gets or sets requested URI rewriten by routing defaults.
		/// </summary>
		Uri UriRewrite { get; set; }

		/// <summary>
		/// Address to the remote end point
		/// </summary>
		EndPoint RemoteEndPoint { get; set; }

		/// <summary>
		/// Submitted form items
		/// </summary>
		IParameterCollection Form { get; set; }

		/// <summary>
		/// Submitted files
		/// </summary>
		IHttpFileCollection Files { get; set; }

		/// <summary>
		/// Included cookies.
		/// </summary>
		IHttpCookieCollection Cookies { get; set; }

		/// <summary>
		/// Create a response for this request.
		/// </summary>
		/// <returns>Response</returns>
		IHttpResponse CreateResponse();
	}
}