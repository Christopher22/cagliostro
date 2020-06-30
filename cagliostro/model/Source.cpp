/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Source.h"

#include <QFile>
#include <QBuffer>

#ifndef NO_CRYPTO
#include <cryptopp/cryptlib.h>
#include <cryptopp/secblock.h>
#include <cryptopp/aes.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/sha.h>
#include <cryptopp/gcm.h>
#include <cryptopp/filters.h>
#include <cryptopp/zinflate.h>
#include <cryptopp/zdeflate.h>
#endif

namespace cagliostro::model {

Source::Source(QIODevice *device, QObject *parent) : QObject(parent), source_(device) {
  source_->setParent(this);
}

Source::Source(const QString &file_name, QObject *parent) :
	Source(new QFile(file_name, nullptr), parent) {
}

QIODevice *Source::open(QIODevice::OpenMode mode) {
  if (source_ == nullptr || (!source_->isOpen() && !source_->open(mode)) || !source_->seek(0)) {
	return nullptr;
  }

  // Extensive amounts of whitespace in the beginning may make this unreliable
  return Source::isCagliostroHeader(source_->peek(16)) ? source_ : this->decrypt(mode);
}

void Source::setPassword(QStringView password) noexcept {
  if (!password.isEmpty()) {
	password_ = password.toUtf8();
  } else {
	password_ = QByteArray();
  }
}

bool Source::isCagliostroHeader(const QByteArray &header) noexcept {
  const auto trimmed_header = header.trimmed(), EXPECTED = QByteArray("<cagliostro ");
  if (trimmed_header.size() <= 3) {
	return false;
  }

  for (int i = 0, size = std::min(trimmed_header.size(), EXPECTED.size()); i < size; ++i) {
	if (trimmed_header[i] != EXPECTED[i]) {
	  return false;
	}
  }
  return true;
}

QIODevice *Source::decrypt(QIODevice::OpenMode mode) {
#ifndef NO_CRYPTO
  return Source::generate_key_iv(password_,
								 [this, mode](unsigned char *key,
											  unsigned char *iv,
											  unsigned int length) -> QIODevice * {
								   // Prepare the decryption engine
								   CryptoPP::GCM<CryptoPP::AES>::Decryption decryptor;
								   decryptor.SetKeyWithIV(key, length, iv, length);

								   // Load complete file in buffer and build an pipeline
								   const auto cipher = source_->readAll();
								   std::string sink;
								   auto *decompressor = new CryptoPP::Inflator(new CryptoPP::StringSink(sink));
								   CryptoPP::AuthenticatedDecryptionFilter decrypt_algorithm(decryptor, decompressor);

								   // Try to decrypt the data
								   try {
									 decrypt_algorithm.Put(reinterpret_cast<const CryptoPP::byte *>(cipher.data()),
														   cipher.size());
									 decrypt_algorithm.MessageEnd();
								   } catch (...) {
									 return nullptr;
								   };

								   // Create a buffer and fill it
								   auto *buffer = new QBuffer(this);
								   buffer->buffer().resize(sink.size());
								   std::memcpy(buffer->buffer().data(), sink.data(), sink.size());
								   if (buffer->open(mode)) {
									 return buffer;
								   } else {
									 buffer->deleteLater();
									 return nullptr;
								   }
								 });
#else
  return nullptr;
#endif
}

QByteArray Source::encrypt(QIODevice *source, QStringView password) {
#ifndef NO_CRYPTO
  if (source == nullptr || !source->isOpen()) {
	return QByteArray();
  }

  auto data = source->readAll();
  if (password.empty()) {
	return data;
  }

  QByteArray encrypted_data;
  Source::generate_key_iv(password.toUtf8(), [&data, &encrypted_data](unsigned char *key,
																	  unsigned char *iv,
																	  unsigned int length) -> QIODevice * {
	// Prepare the encryption engine
	CryptoPP::GCM<CryptoPP::AES>::Encryption encryption;
	encryption.SetKeyWithIV(key, length, iv, length);

	std::string sink;
	auto *encryption_algorithm =
		new CryptoPP::AuthenticatedEncryptionFilter(encryption, new CryptoPP::StringSink(sink));
	CryptoPP::Deflator compressor(encryption_algorithm);

	// Try to encrypt the data
	try {
	  compressor.Put(reinterpret_cast<const CryptoPP::byte *>(data.data()), data.size());
	  compressor.MessageEnd();
	} catch (...) {
	  return nullptr;
	};

	// Copy the data
	encrypted_data.resize(sink.size());
	std::memcpy(encrypted_data.data(), sink.data(), sink.size());
	return nullptr;
  });

  return encrypted_data;
#else
  return QByteArray();
#endif
}

QIODevice *Source::generate_key_iv(const QByteArray &password,
								   std::function<QIODevice *(unsigned char *,
															 unsigned char *,
															 unsigned int)> callback,
								   unsigned int iterations) {
#ifndef NO_CRYPTO
  CryptoPP::SecByteBlock derived_key_iv(32);
  CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> key_generator;
  key_generator.DeriveKey(derived_key_iv.data(),
						  derived_key_iv.size(),
						  0,
						  reinterpret_cast<const CryptoPP::byte *>(password.data()),
						  password.size(),
						  nullptr,
						  0,
						  iterations);

  return callback(derived_key_iv.data(), derived_key_iv.data() + 16, 16);
#else
  return nullptr;
#endif
}

}

