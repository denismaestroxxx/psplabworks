using System.Security.Cryptography.X509Certificates;
using System.Security.Cryptography;

namespace Server
{
    internal class Program
    {
        static void Main(string[] args)
        {
            var options = new GenerateCertificateOptions
    (
        pathToSave: "d:\\",
        commonName: "localhost",
        fileName: "IdentityServer4_certificate",
        password: "P@55w0rd",
        5
    );
            MakeCert(options);
            var httpServer = new HttpServer();
            httpServer.Start();
        }

        public class GenerateCertificateOptions
        {
            public GenerateCertificateOptions(string pathToSave, string commonName, string fileName, string password, int years = 1)
            {
                CommonName = commonName;
                if (string.IsNullOrEmpty(CommonName))
                {
                    throw new ArgumentNullException(nameof(CommonName));
                }

                PathToSave = pathToSave;
                if (string.IsNullOrEmpty(PathToSave))
                {
                    throw new ArgumentNullException(nameof(PathToSave));
                }

                Password = password;
                if (string.IsNullOrEmpty(Password))
                {
                    throw new ArgumentNullException(nameof(Password));
                }

                CertificateFileName = fileName;
                if (string.IsNullOrEmpty(CertificateFileName))
                {
                    throw new ArgumentNullException(nameof(CertificateFileName));
                }

                Years = years;
                if (Years <= 0)
                {
                    Years = 1;
                }
            }

            public string CommonName { get; }
            public string PathToSave { get; }
            public string Password { get; }
            public string CertificateFileName { get; }
            public int Years { get; }
        }

        static void MakeCert(GenerateCertificateOptions options)
        {
            var rsa = RSA.Create(4096);
            var req = new CertificateRequest($"cn={options.CommonName}", rsa, HashAlgorithmName.SHA256, RSASignaturePadding.Pkcs1);
            var cert = req.CreateSelfSigned(DateTimeOffset.Now, DateTimeOffset.Now.AddYears(options.Years));
            var path = Path.Combine(options.PathToSave, options.CertificateFileName);

            // Create PFX (PKCS #12) with private key
            File.WriteAllBytes($"{path}.pfx", cert.Export(X509ContentType.Pfx, options.Password));

            // Create Base 64 encoded CER (public key only)
            File.WriteAllText($"{path}.cer",
                "-----BEGIN CERTIFICATE-----\r\n"
                + Convert.ToBase64String(cert.Export(X509ContentType.Cert), Base64FormattingOptions.InsertLineBreaks)
                + "\r\n-----END CERTIFICATE-----");
        }
    }


}